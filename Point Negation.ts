// Definim valoarea câmpului finit p
const p = 9739;

// Punctul P dat
const P = { x: 8045, y: 6936 };

// Calculăm punctul Q, inversul lui P
// Q = (x, -y mod p)
const Q = {
    x: P.x,
    y: (p - P.y) % p // Calculăm -y mod p
};

console.log(`Punctul Q este: (${Q.x}, ${Q.y})`);
