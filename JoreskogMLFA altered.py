import numpy as np
import pandas as pd
from numpy.linalg import inv, slogdet
from scipy.stats import chi2

np.random.seed(20240220)
np.set_printoptions(suppress=True, precision=3)

def LargestK(A: np.array, k: int):
    eigvals, eigvcts = np.linalg.eig(A)
    idx = np.sort(np.argpartition(np.array(eigvals), -k)[-k:])
    return eigvals[idx], eigvcts[:, idx]

def ExceptLargestK(A: np.array, k: int):
    eigvals, eigvcts = np.linalg.eig(A)
    idx = np.sort(np.argpartition(np.array(eigvals), -k)[:-k])
    return eigvals[idx], eigvcts[:, idx]

class JoreskogMLFA(object):
    def __init__(self, data: pd.DataFrame, k: int, tol = 1e-8):
        self.data = data.T
        self.k = k
        self.p = len(self.data.index)
        self.n = len(self.data.columns)
        self.S = self.data.T.corr().values
        self.epi = tol

    def Grad(self, x: np.array):
        x_ls = (x.T)[0]
        Theta, Omega = LargestK(np.diag(x_ls**(-0.5)) @ self.S @ np.diag(x_ls**(-0.5)), self.k)
        # print(Omega.T @ Omega)
        g_ls = [( sum([(Theta[m] - 1) * Omega[i, m]**2 
                        for m in range(self.k)]) 
                                + 1 - self.S[i, i]/x_ls[i] ) / x_ls[i] 
                                        for i in range(self.p)]
        for i in range(self.p):
            if x_ls[i] < 2e-4:
                g_ls[i] = 0
        return np.array([g_ls]).T

    def Epoach(self, x: np.array, g: np.array, E: np.array):
        d = - E @ g

        alpha = 0
        if (d < 0).any():
            alpha_max = min([(self.epi - x[i, 0]) / d[i, 0] 
                                            for i in range(self.p) if d[i, 0] < 0])
        else:
            alpha_max = 1e8
        grad = lambda alp: (self.Grad(x+alp * d).T @ d)[0, 0]
        while grad(alpha) <= 0 and alpha < alpha_max - 1e-3:
            alpha += 1e-3
        if grad(0) == grad(alpha):
            return x, g, E, True
        alpha = min(grad(0) * alpha / (grad(0) - grad(alpha)), alpha_max)

        new_x = x + alpha * d
        new_g = self.Grad(new_x)
        y = new_x - x
        h = new_g - g
        new_E = E + (y @ y.T) / (y.T @ h)[0, 0] - E @ h @ h.T @ E / (h.T @ E @ h)[0, 0]

        for i in range(self.p):
            if new_x[i, 0] < 2e-4:
                for j in range(self.p):
                    if j != i:
                        new_E[i, j] = 0
                        new_E[j, i] = 0
        return new_x, new_g, new_E, False

    def run(self):
        S_inv = inv(self.S)
        Phi_ls = np.array([max((1 - self.k / (2*self.p)) / S_inv[i, i], self.epi) for i in range(self.p)])
        _, Omega = LargestK(np.diag(Phi_ls**(-0.5)) @ self.S @ np.diag(Phi_ls**(-0.5)), self.k)
        x, E = [ np.array([Phi_ls]).T ], [ (np.diag(Phi_ls**(-0.5)) @ (np.eye(self.p) - Omega @ Omega.T) @ np.diag(Phi_ls**(-0.5))) ** 2 ]
        for i in range(self.p):
            if x[0][i, 0] < 2 * self.epi:
                for j in range(self.p):
                    if j != i:
                        E[0][i, j] = 0
                        E[0][j, i] = 0
        g = [self.Grad(x[0])]

        s = 0
        while np.max(np.abs(self.Grad(x[s]))) > self.epi:
            print(f"on {s}: ", np.max(self.Grad(x[s])))
            new_x, new_g, new_E, judge = self.Epoach(x[s], g[s], E[s])
            if judge:
                break
            x.append(new_x)
            g.append(new_g)
            E.append(new_E)
            s += 1

        Phi = np.diag((x[s].T)[0])
        Theta, Omega = LargestK(np.diag(((x[s].T)[0])**(-0.5)) @ self.S @ np.diag(((x[s].T)[0])**(-0.5)), self.k)
        Theta = np.diag(Theta)
        Lambda = np.diag(((x[s].T)[0])**0.5) @ Omega @ (Theta - np.eye(self.k)) ** 0.5
        F = inv(Lambda.T @ inv(Phi) @ Lambda) @ Lambda.T @ inv(Phi) @ self.data.values
        U = self.data.values - Lambda @ F
        Sigma = Lambda @ Lambda.T + Phi

        return Phi, Lambda, F, U, Sigma
