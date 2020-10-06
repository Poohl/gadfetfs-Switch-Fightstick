
uint pos;
key* sequence;
uint8 c_layer, c_column, c_row;

typedef struct {
	char key;
	uint8 layer;
	union {
		 struct {
			 uint8 column, row;
		 };
		 uint16 quick_key;
	 }
} key;

key keyboard[] = {
	{0}
}

key layerTransitions[][] = {
	{
		{
			.key = 0,
			.layer = TYPE_LAYER_QUICK,
			.quick_key = SWITCH_LCLICK
		},
		{
			.key = 0,
			.layer =
		}
	}
}

void type_call(char* args) {
	sequence = (char*) malloc(strlen(args) * sizeof(key));
	for (int i = 0; args[i]; ++i) {
		int j;
		for (j = 0; j < sizeof(keyboard) / sizeof(keyboard[0]) && keyboard[j].key != args[i]; j++);
		if (j < sizeof(keyboard) / sizeof(keyboard[0])) {
			sequence[i] = keyboard[j];
		} else {
			free(sequence);
			sequence = NULL;
			return;
		}
	}
}

void type(key* k, USB_JoystickReport_Input_t* dest) {
	if (k->layer == TYPE_LAYER_QUICK) {
		dest->Button |= k->quick_key;
	} else {
		bool correctPosition = 1;
		if (k->layer != c_layer) {
			dest->Button |= layerTransitions[c_layer][k->layer];
			correctPosition = 0;
		}
		uint8 hat = 0;
		if (k->row < c_row) {
			hat |= 0b0001;
			--c_row;correctPosition = 0;
		}
		if (k->row > c_row) {
			hat |= 0b0100;
			++c_row;correctPosition = 0;
		}
		if (k->column < c_column) {
			hat |= 0b1000;
			--c_column;correctPosition = 0;
		}
		if (k->column > c_column) {
			hat |= 0b0010;
			++c_column;correctPosition = 0;
		}
		if (!correctPosition) {
			dest->Hat = fillHat(buttonToHat(hat));
		} else {
			dest->Button |= SWITCH_A;
		}
	}
}

void type_frame(CommandArgs* args) {
	if (sequence) {
		type()
	}
}

CommandHook mariomaker_hook = {&mariomaker_call, &mariomaker_frame, "mariomaker"};
CommandHook mariomaker_hook = {&mariomaker_call, &mariomaker_frame, "mm"};
