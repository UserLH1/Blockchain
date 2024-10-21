// Funcția pentru calculul modular invers folosind Algoritmul Euclidian Extins
function modInverse(e: bigint, phi: bigint): bigint {
  let m0 = phi, t, q;
  let x0 = BigInt(0), x1 = BigInt(1);

  if (phi === BigInt(1)) return BigInt(0);

  // Aplicați Algoritmul Euclidian Extins
  while (e > BigInt(1)) {
    q = e / phi;
    t = phi;

    phi = e % phi;
    e = t;
    t = x0;

    x0 = x1 - q * x0;
    x1 = t;
  }

  // Asigurați-vă că x1 este pozitiv
  if (x1 < BigInt(0)) x1 += m0;

  return x1;
}

// Datele furnizate
const p = BigInt("857504083339712752489993810777");
const q = BigInt("1029224947942998075080348647219");
const e = BigInt(65537);

// Calculăm N și φ(N)
const N = p * q;
const phiN = (p - BigInt(1)) * (q - BigInt(1));

// Calculăm cheia privată d
const d = modInverse(e, phiN);

console.log(`Cheia privată d este: ${d}`);
