#!/usr/bin/env python3
from PIL import Image
import os
import sys
import uuid

#Config
THUMBNAIL_SIZE=(32,32)
ROOT_DIR="c:/John_Wood/Coding_Projects/c/KVDB"
DIR_PATH=ROOT_DIR+"/res/images"
EXPORT_PATH=ROOT_DIR+"/thumbnails"
TXT_FILE_PATH=EXPORT_PATH+"/thumbnames.txt"
EXT_ARR=(".png",".jpg",".jpeg",".bmp",".webp")
#EndConfig

def main():
	sh = ("rm -rf \"" + EXPORT_PATH + "\"")
	print(sh)
	os.system(sh)
	sh = ("mkdir \"" + EXPORT_PATH + "\"")
	print(sh)
	os.system(sh)
	#for filename in os.listdir(EXPORT_PATH):
	#	sh = ("rm " + EXPORT_PATH + '/' + f"{filename}")
	#	print(sh)
	#	os.system(sh)
	#endfor
	os.system(f"echo. > {TXT_FILE_PATH}")
	f_txt = open(TXT_FILE_PATH, "w")

	f_txt.write("const char* filenames[] = {\n")
	dir_ls = os.listdir(DIR_PATH)
	dir_entry_count = len(dir_ls)
	output_count = 0
	for i in range(0, dir_entry_count):
		filename = dir_ls[i]
		if (filename.lower().endswith(EXT_ARR)):
			img_path = (DIR_PATH + '/' + filename)
			img = Image.open(img_path)
			if (not img):
				print(f"Failed to open file \"{img_path}\"")
				continue
			#endif
			img.thumbnail(THUMBNAIL_SIZE)
			random_name = f"{uuid.uuid4().hex}.png"
			img.save(EXPORT_PATH + "/" + random_name)
			f_txt.write("\t")
			f_txt.write("\"thumbnails/" + random_name + '\"')
			if (i < dir_entry_count - 1):
				f_txt.write(",\n")
			else:
				f_txt.write("\n")
			#endif
			print("\x1b[32m" "Saved " f"{random_name}" "\x1b[0m")
			img.close()
			output_count += 1
		#endif
	#endfor
	f_txt.write("};\n")
	f_txt.write("\n#define ENTRY_COUNT sizeof(filenames) / sizeof(char*)\n")
	f_txt.close()
#endef

if (__name__ == "__main__"):
	main()
#endif
