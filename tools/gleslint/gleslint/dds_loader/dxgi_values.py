# BPP and block sizes for all DXGI formats
# Since dxgi formats are enumerated from 0 onward there is no need for dictionary
# if some formats are not suited for storing the value is going to be set to 0
# Sizes are in BYTES
dxgi_pixel_or_block_size = [
	0, 
	16, 16, 16, 16, 
	12, 12, 12, 12, 
	8, 8, 8, 8, 8, 8, 
	8, 8, 8, 8, 
	8, 8, 8, 8, 
	4, 4, 4, 4, 
	4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 
	4, 4, 4, 4, 4, 
	4, 
	4, 4, 4, 
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
	1, 1, 1, 1, 1, 1, 
	0, # DXGI_FORMAT_R1_UNORM ehm >.< ( TODO )
	4, 4, 4,
	8, 8, 8,    # BC1
	16, 16, 16, # BC2
	16, 16, 16, # BC3
	8, 8, 8, 	# BC4
	16, 16, 16, # BC5
	2, 2, 
	4, 4, 4, 4, 4, 4, 4, 
	16, 16, 16, # BC6
	16, 16, 16, # BC7
	# TODO Complete the rest
]

dxgi_compressed_formats = [
	70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
	94, 95, 96, 97, 98, 99
]