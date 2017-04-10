#include <stdio.h>
#include <easy_string.h>

int main() {
	//byte => hex
	{
		char bytes[] = {0x12, 0x21, 0x44, 0x67, 0xf3, 0xa4};
		char buf[20] = {0};
		easy_string_tohex(bytes, sizeof(bytes), buf, sizeof(buf));
		printf("hex: %s\n", buf);
	}
	//upper && lower
	{
		char str[] = "aR67Yuido43";
		printf("%d => %d\n", 'a', 'A');
		printf("ori: %s\n", str);
		easy_string_toupper(str);
		printf("upper: %s\n", str);
		easy_string_tolower(str);
		printf("lower: %s\n", str);
	}
	//strcpy
	{
		char *src = "Hello, world! How are you?";
		char dst[30] = {0};
		easy_strncpy(dst, src, sizeof(dst));
		printf("src: %s\n", src);
		printf("dst: %s\n", dst);
	}
	return 0;
}
