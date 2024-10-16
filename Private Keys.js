// Funcția pentru calculul modular invers folosind Algoritmul Euclidian Extins
function modInverse(e, phi) {
    var m0 = phi, t, q;
    var x0 = BigInt(0), x1 = BigInt(1);
    if (phi === BigInt(1))
        return BigInt(0);
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
    if (x1 < BigInt(0))
        x1 += m0;
    return x1;
}
// Datele furnizate
var p = BigInt("857504083339712752489993810777");
var q = BigInt("1029224947942998075080348647219");
var e = BigInt(65537);
// Calculăm N și φ(N)
var N = p * q;
var phiN = (p - BigInt(1)) * (q - BigInt(1));
// Calculăm cheia privată d
var d = modInverse(e, phiN);
console.log("Cheia privat\u0103 d este: ".concat(d));
