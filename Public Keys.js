var M = 12;
var exponent = 65537;
var p = 17;
var q = 23;
var n = p * q;
function encrypt(base, exp, mod) {
    var result = 1;
    base = base % mod;
    while (exp > 0) {
        // Dacă exponentul este impar, înmulțim cu baza
        if (exp % 2 === 1) {
            result = (result * base) % mod;
        }
        // Împărțim exponentul la 2
        exp = Math.floor(exp / 2);
        // Ridicăm baza la pătrat
        base = (base * base) % mod;
    }
    return result;
}
console.log(encrypt(M, exponent, n));
