/*
void down(char* buff, uint len) {
	for (char* p = "down"; *p == *buff; ++p, ++buff);
	if (!*buff || (*buff == ' ' && !*(buff += 1))) {
		printf("No button specified\n");
		return;
	}
	int index = get_button_index(buff);
	if (index < 14) {
		queue[next].Button |= 1 << index;
	} else {
		printf("Hat siwtch, dunno\n");
	}
}

void up(char* buff, uint len) {
	for (char* p = "down"; *p == *buff; ++p, ++buff);
	if (!*buff || (*buff == ' ' && !*(buff += 1))) {
		printf("No button specified\n");
		return;
	}
	int index = get_button_index(buff);
	if (index < 14) {
		queue[next].Button |= 1 << index;
	} else {
		printf("Hat siwtch, dunno\n");
	}
}*/
