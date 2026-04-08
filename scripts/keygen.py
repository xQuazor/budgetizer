#!/usr/bin/env python3
"""
Generate a fresh RSA key pair in JUCE-compatible format.

Run this once to rotate keys (e.g. if the private key leaks):
  python3 scripts/keygen.py

Then:
  1. Replace PUBLIC_KEY in Source/license/LicenseValidator.h with the new public key.
  2. Replace PRIVATE_KEY in scripts/sign_license.py with the new private key.
  3. Rebuild the plugin — all previously issued keys become invalid.
"""
import random
import sys


def miller_rabin(n, k=25):
    if n < 2: return False
    if n in (2, 3): return True
    if n % 2 == 0: return False
    r, d = 0, n - 1
    while d % 2 == 0:
        r += 1
        d //= 2
    for _ in range(k):
        a = random.randrange(2, n - 1)
        x = pow(a, d, n)
        if x in (1, n - 1):
            continue
        for _ in range(r - 1):
            x = pow(x, 2, n)
            if x == n - 1:
                break
        else:
            return False
    return True


def gen_prime(bits):
    while True:
        n = random.getrandbits(bits) | (1 << (bits - 1)) | 1
        if miller_rabin(n):
            return n


def generate_keys(bits=1024):
    half = bits // 2
    print(f"Generating {bits}-bit key pair…", file=sys.stderr)
    p = gen_prime(half)
    q = gen_prime(half)
    while q == p:
        q = gen_prime(half)
    n = p * q
    e = 65537
    phi = (p - 1) * (q - 1)
    d = pow(e, -1, phi)
    pub  = f"{hex(e)[2:]},{hex(n)[2:]}"
    priv = f"{hex(d)[2:]},{hex(n)[2:]}"
    return pub, priv


if __name__ == "__main__":
    pub, priv = generate_keys()
    print(f"\nPUBLIC KEY  (embed in Source/license/LicenseValidator.h):\n{pub}")
    print(f"\nPRIVATE KEY (keep secret — put in scripts/sign_license.py):\n{priv}")
