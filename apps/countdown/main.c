int num = 0xdead;
int sum;
char *name = "zaharije pasalic";

int increment() {
	sum = num;
	return num++;
}

int main() {
	while(1) {
		increment();
	}
	return 0;
}