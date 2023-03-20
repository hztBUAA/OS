#include <print.h>

/* forward declaration */
static void print_char(fmt_callback_t, void *, char, int, int);
static void print_str(fmt_callback_t, void *, const char *, int, int);
static void print_num(fmt_callback_t, void *, unsigned long, int, int, int, int, char, int);

void vprintfmt(fmt_callback_t out, void *data, const char *fmt, va_list ap) {
	char c;
	const char *s;
	long num;

	int width;
	int long_flag; // output is long (rather than int)
	int neg_flag;  // output is negative
	int ladjust;   // output is left-aligned
	char padc;     // padding char 默认为空格  即用来补位的字符
	int prec;
	for (;*fmt;) {//第一次上交时这里的第二个参数没写 导致错误？仔细分析了一下 确实  会导致输出的数据减少  但还是要会debug
		/* scan for the next '%' */
		/* Exercise 1.4: Your code here. (1/8) */
		if ( *fmt != '%')
		{
			out(data,fmt,1);
			fmt++;
			continue;	
		}
		/* flush the string found so far */
		/* Exercise 1.4: Your code here. (2/8) */

		/* check "are we hitting the end?" */
		/* Exercise 1.4: Your code here. (3/8) */

		/* we found a '%' */
		/* Exercise 1.4: Your code here. (4/8) */
		fmt++;
		long_flag = 0;
		neg_flag = 0;
		width = 0;
		prec = 0;
		ladjust = 0;
		padc = ' ';
		/* check format flag */
		/* Exercise 1.4: Your code here. (5/8) */
		
		/* get width */
		/* Exercise 1.4: Your code here. (6/8) */

		/* check for long */
		/* Exercise 1.4: Your code here. (7/8) */
		if (*fmt == '-')
			ladjust = 1, fmt++;/*
			*左对齐 formatflag包括- 0
			*/
		if ((*fmt) == '0')//这里的0表示占位符变成0
			padc = '0', fmt++;
		while((*fmt)>='0'&&(*fmt)<='9')
		{//表示输出的宽度
			width = 10*width + (*fmt)-'0';
			fmt++;
		}
		if ((*fmt) == 'l')//表示是long long 类型
			long_flag = 1, fmt++;
		neg_flag = 0;


//*****************************开始确定具体类型********************
		switch (*fmt) {
			case 'R':
				if(long_flag){
					num = va_arg(ap,long int);

				}
				else{
					num = va_arg(ap,int);
				}
				if (num < 0)
	                                 neg_flag = 1, num = -num;
				char c = '(';
				out(data,&c,1);
				print_num(out,data,num,10,neg_flag,width,ladjust,padc,0);
				char d = ',';
				out(data,&d,1);
				neg_flag = 0;
				if(long_flag){
                                         num = va_arg(ap,long int);
                                  }
                                 else{
                                         num = va_arg(ap,int);
                                 }
				 if(num <0)
					 neg_flag = 1,num = -num;
				 else
					 neg_flag = 0,num = num;
				 print_num(out,data,num,10,neg_flag,width,ladjust,padc,0);
				 char e = ')';
				 out(data,&e,1);
				 
				 
				 break;
			
		case 'b':/**
			   无符号二进制*/
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}
			//print_num(out,data,[specified var],radix,negflag,width,ladjust,padc,upcase(表示输出数字十六进制中的字母时是否大小写));
			print_num(out, data, num, 2, 0, width, ladjust, padc, 0);
			break;//question：这里break以后还会再次不断运行这个vfmtprint函数？毕竟现在break已经退出了这个for？answer：脑袋犯病了 这个是只退出switch

		case 'd':
		case 'D':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}

			/*
			 * Refer to other parts (case 'b', case 'o', etc.) and func 'print_num' to
			 * complete this part. Think the differences between case 'd' and the
			 * others. (hint: 'neg_flag').
			 */
			/* Exercise 1.4: Your code here. (8/8) */
			if (num < 0)
				neg_flag = 1, num = -num;
			print_num(out, data, num, 10, neg_flag, width, ladjust, padc, 0);
			break;

		case 'o':
		case 'O':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}
			print_num(out, data, num, 8, 0, width, ladjust, padc, 0);
			break;

		case 'u':
		case 'U':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}
			print_num(out, data, num, 10, 0, width, ladjust, padc, 0);
			break;

		case 'x':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}
			print_num(out, data, num, 16, 0, width, ladjust, padc, 0);
			break;

		case 'X':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}
			print_num(out, data, num, 16, 0, width, ladjust, padc, 1);
			break;

		case 'c':
			c = (char)va_arg(ap, int);
			print_char(out, data, c, width, ladjust);
			break;

		case 's':
			s = (char *)va_arg(ap, char *);
			print_str(out, data, s, width, ladjust);
			break;

		case '\0':
			fmt--;
			break;

		default:
			/* output this char as it is */
			out(data, fmt, 1);//out将fmt字符一字节输出到data地址的内存上
		}
		fmt++;
	}
}

/* --------------- local help functions --------------------- */
void print_char(fmt_callback_t out, void *data, char c, int length, int ladjust) {
	int i;

	if (length < 1) {
		length = 1;
	}
	const char space = ' ';
	if (ladjust) {/*左对齐*/
		out(data, &c, 1);
		for (i = 1; i < length; i++) {
			out(data, &space, 1);/*out将space的值输出到data中以1字节*/
		}
	} else {
		for (i = 0; i < length - 1; i++) {
			out(data, &space, 1);
		}
		out(data, &c, 1);
	}
}
/**
 *这里的length参数和上面vfmtprint参数的length不是一个意思 width
 */
void print_str(fmt_callback_t out, void *data, const char *s, int length, int ladjust) {
	int i;
	int len = 0;
	const char *s1 = s;
	while (*s1++) {
		len++;
	}
	if (length < len) {
		length = len;
	}

	if (ladjust) {
		out(data, s, len);
		for (i = len; i < length; i++) {
			out(data, " ", 1);
		}
	} else {
		for (i = 0; i < length - len; i++) {
			out(data, " ", 1);
		}
		out(data, s, len);
	}
}

void print_num(fmt_callback_t out, void *data, unsigned long u, int base, int neg_flag, int length,
	       int ladjust, char padc, int upcase) {
	/* algorithm :
	 *  1. prints the number from left to right in reverse form.
	 *  2. fill the remaining spaces with padc if length is longer than
	 *     the actual length
	 *     TRICKY : if left adjusted, no "0" padding.
	 *		    if negtive, insert  "0" padding between "0" and number.
	 *  3. if (!ladjust) we reverse the whole string including paddings
	 *  4. otherwise we only reverse the actual string representing the num.
	 */

	int actualLength = 0;
	char buf[length + 70];
	char *p = buf;
	int i;

	do {
		int tmp = u % base;
		if (tmp <= 9) {
			*p++ = '0' + tmp;
		} else if (upcase) {
			*p++ = 'A' + tmp - 10;
		} else {
			*p++ = 'a' + tmp - 10;
		}
		u /= base;
	} while (u != 0);

	if (neg_flag) {
		*p++ = '-';
	}

	/* figure out actual length and adjust the maximum length */
	actualLength = p - buf;
	if (length < actualLength) {
		length = actualLength;
	}

	/* add padding */
	if (ladjust) {
		padc = ' ';
	}
	if (neg_flag && !ladjust && (padc == '0')) {
		for (i = actualLength - 1; i < length - 1; i++) {
			buf[i] = padc;
		}
		buf[length - 1] = '-';
	} else {
		for (i = actualLength; i < length; i++) {
			buf[i] = padc;
		}
	}

	/* prepare to reverse the string */
	int begin = 0;
	int end;
	if (ladjust) {
		end = actualLength - 1;
	} else {
		end = length - 1;
	}

	/* adjust the string pointer */
	while (end > begin) {
		char tmp = buf[begin];
		buf[begin] = buf[end];
		buf[end] = tmp;
		begin++;
		end--;
	}

	out(data, buf, length);
}
