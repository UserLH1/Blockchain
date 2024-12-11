pragma solidity ^0.8.0;

import "forge-std/Test.sol";
import "../UniswapV3Factory.sol";
import "../UniswapV3Pool.sol";
import "@openzeppelin/contracts/token/ERC20/ERC20.sol";

contract MyCustomToken is ERC20 {
    constructor(string memory name, string memory symbol) ERC20(name, symbol) {}

    function mint(address to, uint256 amount) external {
        _mint(to, amount);
    }
}

contract CustomPoolSwap is Test {
    UniswapV3Factory factory;
    UniswapV3Pool pool;
    MyCustomToken tokenA;
    MyCustomToken tokenB;

    function setUp() public {
        // Deploy factory
        factory = new UniswapV3Factory();

        // Deploy custom tokens
        tokenA = new MyCustomToken("TokenA", "TKA");
        tokenB = new MyCustomToken("TokenB", "TKB");

        // Mint tokens to this contract
        tokenA.mint(address(this), 1_000_000 * 10**18);
        tokenB.mint(address(this), 1_000_000 * 10**18);

        // Create the pool with 0.3% fee
        pool = UniswapV3Pool(
            factory.createPool(address(tokenA), address(tokenB), 3000)
        );

        // Initialize the pool with a starting price (sqrtPriceX96)
        pool.initialize(79228162514264337593543950336); // sqrt(1) in Q64.96 format
    }
}