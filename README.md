# BinDiff
This application is intended to make a diff files for the backup purposes.
Imagine that you have a VM with 10GB vmdk file that you backup every day (same for Hyper-V vhd/vhdx files). So, you need to keep 100GB (let say 60GB with compression) for 10 days. However from those 10GB file only ~500MB of data is daily changed and you can shrink your backup storage to 15GB, if you keep only the daily difference info. The BinDiff makes this job for you.

Initially you make a full copy of vmfk file (let's say every month or two). That time you also make a hash file to track the block changes. You keep the hash file (which is small enough) and every day make a differential file from the vmdk file that you keep on the backup storage. Profit!

For the recovery BinDiff merges the latest available diff file with the initial vmdk file and gives you the full vmdk file on that day.
You can also make/check md5 checksums if necessary.

Note that BinDiff tracks 4kB block changes only (VMware/Hyper-V does it, other hypervisors need check).

Features:
provides VMware/Hyper-V backup files deduplication
calculates CRC32 for files
