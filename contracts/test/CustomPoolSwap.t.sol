// SPDX-License-Identifier: MIT
pragma solidity =0.7.6;
pragma abicoder v2;

import "../../lib/forge-std/src/Test.sol";
import "../../lib/forge-std/src/console2.sol";
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
    address token0;
    address token1;

    uint256 initialBalance0;
    uint256 initialBalance1;

    function setUp() public {
        factory = new UniswapV3Factory();
        console2.log("Factory initialized with fee 0.3%");

        tokenA = new MyCustomToken("TokenA", "TKA");
        tokenB = new MyCustomToken("TokenB", "TKB");
        console2.log("TokenA and TokenB deployed");

        tokenA.mint(address(this), 1_000_000 * 10**18);
        tokenB.mint(address(this), 1_000_000 * 10**18);
        console2.log("1 milion TokenA and TokenB minted to the contract");

        address poolAddress = factory.createPool(address(tokenA), address(tokenB), 3000);
        pool = UniswapV3Pool(poolAddress);
        console2.log("Uniswap V3 Pool created at address:", poolAddress);

        token0 = pool.token0();
        token1 = pool.token1();
        console2.log("Token0:", token0);
        console2.log("Token1:", token1);

        uint160 sqrtPriceX96 = uint160(2**96);
        pool.initialize(sqrtPriceX96);
        console2.log("Pool initialized with sqrtPriceX96:", uint256(sqrtPriceX96));

        IERC20(token0).approve(poolAddress, type(uint256).max);
        IERC20(token1).approve(poolAddress, type(uint256).max);
        console2.log("Pool approved to spend Token0 and Token1");

        int24 tickLower = -600;
        int24 tickUpper = 600;

        uint128 liquidity = 100000 * 10**18; // o sută de mii de unitati
        console2.log("Adding liquidity with tickLower:");
        console2.log(uint256(tickLower));
        console2.log("tickUpper:");
        console2.log(uint256(tickUpper));
        console2.log("liquidity:");
        console2.log(uint256(liquidity));

        pool.mint(address(this), tickLower, tickUpper, liquidity, "");
        console2.log("Liquidity minted to the pool");

        initialBalance0 = IERC20(token0).balanceOf(address(this));
        initialBalance1 = IERC20(token1).balanceOf(address(this));
        console2.log("Initial Balance0:", initialBalance0);
        console2.log("Initial Balance1:", initialBalance1);
    }

    function uniswapV3MintCallback(uint256 amount0Owed, uint256 amount1Owed, bytes calldata) external {
        require(msg.sender == address(pool), "Not pool");
        console2.log("uniswapV3MintCallback called");
        console2.log("amount0Owed:", amount0Owed);
        console2.log("amount1Owed:", amount1Owed);

        if (amount0Owed > 0) {
            require(IERC20(token0).transfer(msg.sender, amount0Owed), "Transfer of token0 failed");
            console2.log("Transferred", amount0Owed, "Token0 to pool");
        }
        if (amount1Owed > 0) {
            require(IERC20(token1).transfer(msg.sender, amount1Owed), "Transfer of token1 failed");
            console2.log("Transferred", amount1Owed, "Token1 to pool");
        }
    }

    function uniswapV3SwapCallback(int256 amount0Delta, int256 amount1Delta, bytes calldata) external {
        require(msg.sender == address(pool), "Not pool");
        console2.log("uniswapV3SwapCallback called");
        console2.log("amount0Delta:", amount0Delta);//dam 100
        console2.log("amount1Delta:", amount1Delta);//primim 99.6

        if (amount0Delta > 0) {
            require(IERC20(token0).transfer(msg.sender, uint256(amount0Delta)), "Transfer of token0 failed");
            console2.log("Transferred", uint256(amount0Delta), "Token0 to pool");
        }

        if (amount1Delta > 0) {
            revert("Unexpected amount1Delta > 0 in swap callback");
        }
    }

    function testSwap() public {
        console2.log("Trying to swap:");
        console2.log("zeroForOne:", true);

        int256 amountSpecified = int256(100 * 10**18); // Swap 100 tokens

        uint160 sqrtPriceLimitX96 = 79000000000000000000000000000;//swap-ul se va executa atâta cat nu scade sub 0.99425 
        console2.log("amountSpecified:", uint256(amountSpecified));
        console2.log("sqrtPriceLimitX96:", uint256(sqrtPriceLimitX96));

        (uint160 currentSqrtPrice,,,,,,) = pool.slot0();
        console2.log("currentSqrtPrice before swap:", uint256(currentSqrtPrice));

        pool.swap(
            address(this),
            true, // zeroForOne = true
            amountSpecified,
            sqrtPriceLimitX96,
            ""
        );
        console2.log("Swap executed");

        // Obținerea balanțelor după swap
        uint256 balance0 = IERC20(token0).balanceOf(address(this));
        uint256 balance1 = IERC20(token1).balanceOf(address(this));
        console2.log("After swap:");
        console2.log("balance0:", balance0);
        console2.log("balance1:", balance1);

        // Log slot0 după swap
        (uint160 newSqrtPrice,,,,,,) = pool.slot0();
        console2.log("currentSqrtPrice after swap:", uint256(newSqrtPrice));

        // Aserțiuni corectate:
        // balance0 ar trebui să scadă, balance1 ar trebui să crească
        assertLt(balance0, initialBalance0); // balance0 a scăzut
        assertGt(balance1, initialBalance1); // balance1 a crescut
    }
}
