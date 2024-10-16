import * as crypto from 'crypto';


function createHash(firstName: string, lastName: string): string
{
    const word = firstName + "." + lastName + "." + 'SHA256("Blockchain")';
    console.log(word);    
    const hash:string = crypto.createHash('sha256').update(firstName + "."+lastName).digest('hex');
    return hash;
    
}

const firstName = 'Horatiu';
const lastName = 'Lazea';
console.log(createHash(firstName, lastName));