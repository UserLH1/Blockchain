// SPDX-License-Identifier: MIT
pragma solidity = 0.7.6;

import "forge-std/Test.sol";
import "../UniswapV3Factory.sol";
import "../UniswapV3Pool.sol";
import "../../lib/openzeppelin-contracts/contracts/token/ERC20/ERC20.sol";

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
        // Deploy the Uniswap V3 factory
        factory = new UniswapV3Factory();

        // Deploy custom tokens
        tokenA = new MyCustomToken("TokenA", "TKA");
        tokenB = new MyCustomToken("TokenB", "TKB");

        // Mint tokens to this contract
        tokenA.mint(address(this), 1_000_000 * 10**18);
        tokenB.mint(address(this), 1_000_000 * 10**18);

        // Create the pool with a 0.3% fee (3000 in Uniswap V3)
        address poolAddress = factory.createPool(address(tokenA), address(tokenB), 3000);
        pool = UniswapV3Pool(poolAddress);

        // Initialize the pool with a starting price (sqrtPriceX96)
        // For a 1:1 price ratio, use 2^96
        uint160 sqrtPriceX96 = uint160(2**96);
        pool.initialize(sqrtPriceX96);

        // Approve the pool to spend tokens
        tokenA.approve(poolAddress, type(uint256).max);
        tokenB.approve(poolAddress, type(uint256).max);

        // Add initial liquidity
        int24 tickLower = -887272; // Min tick
        int24 tickUpper = 887272;  // Max tick
        uint128 liquidity = 1_000_000 * 10**18;

        pool.mint(
            address(this),
            tickLower,
            tickUpper,
            liquidity,
            ""
        );
    }

    function testSwap() public {
        // Define swap parameters
        bool zeroForOne = true; // Swap tokenA for tokenB
        int256 amountSpecified = int256(1_000 * 10**18); // Amount of tokenA to swap
        uint160 sqrtPriceLimitX96 = 0; // No price limit

        // Perform the swap
        pool.swap(
            address(this),
            zeroForOne,
            amountSpecified,
            sqrtPriceLimitX96,
            ""
        );

        // Check balances after swap
        uint256 balanceA = tokenA.balanceOf(address(this));
        uint256 balanceB = tokenB.balanceOf(address(this));

        // Ensure tokenA balance decreased and tokenB increased
        assertLt(balanceA, 1_000_000 * 10**18);
        assertGt(balanceB, 1_000_000 * 10**18);
    }
}