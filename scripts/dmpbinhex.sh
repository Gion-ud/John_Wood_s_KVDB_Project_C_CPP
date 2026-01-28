#!/usr/bin/bash
input_bin="./rc/anime_arts/HatarakuSaibou.jpg"
output_hex=./tmp/data.hex


xxd -g 1 -u -c 16 $input_bin | cut -d' ' -f2-17 > $output_hex
