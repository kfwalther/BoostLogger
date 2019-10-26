:: This script builds Boost with the following options:

:: address-model: 	64-bit
:: toolset: 		MSVC 14.2 (VS 2019)
:: j4: 				Use 4 cores for parallel build

cd D:\apps\boost\boost_1_71_0

.\b2 address-model=64 toolset=msvc-14.2 --stagedir=stage --build-type=complete -j4 