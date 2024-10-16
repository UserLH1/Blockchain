function modExp(base, exp, mod) {
    let result = BigInt(1);
    base = base % mod;
  
    while (exp > 0) {
      if (exp % BigInt(2) === BigInt(1)) {
        result = (result * base) % mod;
      }
      exp = exp / BigInt(2);
      base = (base * base) % mod;
    }
  
    return result;
  }
  
  // Datele furnizate
  const N = BigInt("882564595536224140639625987659416029426239230804614613279163");
  const e = BigInt(65537);
  const c = BigInt("77578995801157823671636298847186723593814843845525223303932");
  
  // Valorile p și q din care ai calculat cheia privată
  const p = BigInt("857504083339712752489993810777");
  const q = BigInt("1029224947942998075080348647219");
  
  // Calculăm φ(N) = (p - 1) * (q - 1)
  const phiN = (p - BigInt(1)) * (q - BigInt(1));
  
  // Funcția pentru calculul modular invers folosind Algoritmul Euclidian Extins
  function modInverse(e, phi) {
    let m0 = phi, t, q;
    let x0 = BigInt(0), x1 = BigInt(1);
  
    if (phi === BigInt(1)) return BigInt(0);
  
    while (e > BigInt(1)) {
      q = e / phi;
      t = phi;
  
      phi = e % phi;
      e = t;
      t = x0;
  
      x0 = x1 - q * x0;
      x1 = t;
    }
  
    if (x1 < BigInt(0)) x1 += m0;
  
    return x1;
  }
  
  // Calculăm cheia privată d
  const d = modInverse(e, phiN);
  
  // Decriptăm mesajul folosind d și N
  const m = modExp(c, d, N);
  
  console.log(`Mesajul decriptat este: ${m}`);
  