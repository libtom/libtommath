#include <tommath.h>
int main(int argc, char ** argv) {

	const unsigned int a = 65537;
	char b[] = 
"272621192922230283305477639564135351471136149273956463844361347729298759183125368038593484043149128512765280523210111782526587388894777249539002925324108547349408624093466297893486263619517809026841716115227596170065100354451708345238523975900663359145770823068375223714001310312030819080370340176730626251422070";
	char radix[1000];
	mp_int vala, valb, valc;

	if (mp_init(&vala) != MP_OKAY) {
		fprintf(stderr, "failed to init vala\n");
		exit(1);
	}

	if (mp_init(&valb) != MP_OKAY) {
		fprintf(stderr, "failed to init valb\n");
		exit(1);
	}

	if (mp_init(&valc) != MP_OKAY) {
		fprintf(stderr, "failed to init valc\n");
		exit(1);
	}
	if (mp_set_int(&vala, 65537) != MP_OKAY) {
		fprintf(stderr, "failed to set vala to 65537\n");
		exit(1);
	}

	if (mp_read_radix(&valb, b, 10) != MP_OKAY) {
		fprintf(stderr, "failed to set valb to %s\n", b);
		exit(1);
	}

	if (mp_invmod(&vala, &valb, &valc) != MP_OKAY) {
		fprintf(stderr, "mp_invmod failed\n");
		exit(1);
	}

	if (mp_toradix(&valc, radix, 10) != MP_OKAY) {
		fprintf(stderr, "failed to convert value to radix 10\n");
		exit(1);
	}

	fprintf(stderr, "a = %d\nb = %s\nc = %s\n", a, b, radix);
	return 0;
}