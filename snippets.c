
/*
uint8_t _lui_touch_input_event_checker()
{
	uint8_t input_event;
	lui_touch_input_data_t input_data;
	g_lui_main.touch_input_dev->read_touch_input_cb(&input_data);

	// see the flow diagram: https://app.code2flow.com/o5avjJUIFOUE

	if (input_data.x == g_lui_main.last_touch_data.x &&
		input_data.y == g_lui_main.last_touch_data.y)
	{
		if (input_data.is_pressed == 1)
		{
			if (g_lui_main.last_touch_data.is_pressed == 0)
				input_event = LUI_INPUT_EVENT_PRESSED;
			else
				input_event = LUI_INPUT_EVENT_NONE;		
		}
		else
		{
			if (g_lui_main.last_touch_data.is_pressed == 1)
				input_event = LUI_INPUT_EVENT_RELEASED;
			else
				input_event = LUI_INPUT_EVENT_NONE;	
		}	
	}
	else
	{
		if (input_data.is_pressed == 1)
			input_event = LUI_INPUT_EVENT_PRESSMOVED;

		else
			input_event = LUI_INPUT_EVENT_MOVED;
	}

	g_lui_main.last_touch_data.x = input_data.x;
	g_lui_main.last_touch_data.y = input_data.y;
	g_lui_main.last_touch_data.is_pressed = input_data.is_pressed;

	return input_event;
}
*/

// void _lui_process_dpad_input(lui_scene_t *lui_scene)
// {
// 	if (lui_scene->dpad.max_col_pos == -1) // no element is configured for dpad, so return
// 		return;

// 	lui_dpad_input_data_t input;
// 	g_lui_main.dpad_input_dev->read_dpad_input_cb(&input);

// 	if (input.is_right_pressed)	lui_scene->dpad.current_col_pos++;
// 	if (input.is_left_pressed)	lui_scene->dpad.current_col_pos--;
// 	if (input.is_down_pressed)	lui_scene->dpad.current_row_pos++;
// 	if (input.is_up_pressed)	lui_scene->dpad.current_row_pos--;

// 	// check dpad boundary
// 	if (lui_scene->dpad.current_col_pos > lui_scene->dpad.max_col_pos || lui_scene->dpad.current_col_pos < 0)
// 		lui_scene->dpad.current_col_pos = 0;
// 	if (lui_scene->dpad.current_row_pos > lui_scene->dpad.max_row_pos)
// 		lui_scene->dpad.current_row_pos = 0;

// 	uint8_t event;
// 	uint8_t input_on_element = 0; //if the input coordinates on the current elemt or not. if yes, stop scanning other elements & return
// 	for (uint8_t i = 0; i < lui_scene->obj_total; i++)
// 	{
// 		uint8_t new_state = LUI_STATE_IDLE;
// 		if (lui_scene->dpad.current_col_pos == lui_scene->lui_button[i]->dpad_col_pos &&
// 			lui_scene->dpad.current_row_pos == lui_scene->lui_button[i]->dpad_row_pos)
// 		{
// 			input_on_element = 1; 

// 			if (input.is_enter_pressed == 1)
// 				new_state = LUI_STATE_PRESSED;
// 			else
// 				new_state = LUI_STATE_SELECTED;
// 		}
// 		// else if input position is not on button position
// 		else
// 		{
// 			new_state = LUI_STATE_IDLE;
// 		}

// 		event = _lui_get_event_against_state(new_state, lui_scene->lui_button[i]->state);
		
// 		// If new state is not same as the existing state, only then refresh the button
// 		if (event != LUI_EVENT_NONE)
// 		{
// 			lui_scene->lui_button[i]->state = new_state;
// 			lui_scene->lui_button[i]->needs_refresh = 1;
// 			lui_scene->lui_button[i]->event = event;
// 		}
// 		else
// 		{
// 			lui_scene->lui_button[i]->needs_refresh = 0;
// 			lui_scene->lui_button[i]->event = LUI_EVENT_NONE;
// 		}
		

// 		// if input is on this ui element, then no more need to scan other elements. Return now
// 		if (input_on_element == 1)
// 			return;

// 	}

// }


#define LUI_INPUT_EVENT_NONE				0
#define LUI_INPUT_EVENT_MOVED				1
#define LUI_INPUT_EVENT_PRESSED				2
#define LUI_INPUT_EVENT_RELEASED			3
#define LUI_INPUT_EVENT_PRESSMOVED			4



void lui_object_set_position(uint16_t x, uint16_t y, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	if (obj->x == x && obj->y == y)
		return;	

	uint16_t obj_old_x = obj->x;
	uint16_t obj_old_y = obj->y;

	obj->x = x;
	obj->y = y;
	// setting position of children as well.
	for(uint8_t i = 0; i < obj->children_count; i++)
	{
		uint16_t child_new_x = obj->children[i]->x + (obj->x - obj_old_x);
		uint16_t child_new_y = obj->children[i]->y + (obj->y - obj_old_y);
		lui_object_set_position(child_new_x, child_new_y, obj->children[i]);
	}

	_lui_object_set_need_refresh(obj->parent);
}



typedef struct _lui_obj_s
{
	uint16_t x :10;
	uint16_t y :10;
	struct _lui_common_style_s common_style;
	uint8_t state :4;
	uint8_t event :4;
	int32_t value;
	void (*obj_event_cb)(struct _lui_obj_s *obj);
	//private use only
	uint8_t needs_refresh :1;
	int8_t index :6;
	//int8_t parent_index;
	
	uint8_t obj_type :4;

	struct _lui_obj_s *parent;
	struct _lui_obj_s **children;
	uint8_t children_count :5;

	void *obj_main_data;
}lui_obj_t;




// navigation button (prev and next) x,y, w, h set.
	list->nav_btn_nxt->x = obj->x + (obj->common_style.width / 2);
	list->nav_btn_prev->x = obj->x;
	list->nav_btn_nxt->y = list->nav_btn_prev->y = obj->y + list_usable_height;
	list->nav_btn_nxt->common_style.width = list->nav_btn_prev->common_style.width = obj->common_style.width / 2;
	list->nav_btn_nxt->common_style.height = list->nav_btn_prev->common_style.height = glyph_height;
	lui_button_t *button_nav_nxt = list->nav_btn_nxt->obj_main_data;
	lui_button_t *button_nav_prev = list->nav_btn_prev->obj_main_data;
	button_nav_nxt->label.font = button_nav_prev->label.font = list->font;



	_lui_draw_line(obj->x + (obj->common_style.width / 2), obj->y + obj->common_style.width, obj->x + obj->common_style.width - 2, obj->y - 2, 2, chkbox->style.tick_color);




	void _lui_object_set_need_refresh(lui_obj_t *obj)
{
	if (obj == NULL)
		return;

	// already flag is 1, no need to waste time in loop. Return.
	if (obj->needs_refresh == 1)
		return;

	obj->needs_refresh = 1;

	lui_obj_t *child_of_root = obj->first_child;
	while (child_of_root != NULL)
	{
		lui_obj_t *obj_stack[LUI_MAX_OBJECTS] = {NULL};
        uint8_t stack_counter = 0;
        obj_stack[stack_counter++] = child_of_root;
        child_of_root = child_of_root->next_sibling;

		while (stack_counter > 0)
		{
			// pop from stack
			lui_obj_t *child = obj_stack[--stack_counter]; 
			child->needs_refresh = 1;

			// get the child of current object
            child = child->first_child;
			// push all children of current object into stack too
			while (child != NULL)
			{
				// push child to stack
				obj_stack[stack_counter++] = child; 
				// get sibling of the child
                child = child->next_sibling;
			}
		}
	}







	void lui_object_add_to_parent(lui_obj_t *obj_child, lui_obj_t *obj_parent)
{
    if (obj_child == NULL || obj_parent == NULL)
    {
        return;
    }
    
    if (obj_parent->first_child == NULL)
    {
        obj_parent->first_child = obj_child;
    }
    else
    {
        lui_obj_t *next_child = obj_parent->first_child;
        
        while (next_child->next_sibling != NULL)
        {
            next_child = next_child->next_sibling;
        }
        
        next_child->next_sibling = obj_child;
    }
    obj_child->parent = obj_parent;
}
}