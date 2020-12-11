	.globl	dfs_bin_start
dfs_bin_start:
	.incbin "drivers/devfreq/lombo/dfs/dfs.bin"
	.globl	dfs_bin_end
dfs_bin_end:
	.align	2

