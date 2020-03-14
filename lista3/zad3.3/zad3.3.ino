// Imię i Nazwisko: Mateusz Groblicki
// Grupa: Czwartek 11:15
// Zadanie: 3.3. Nawigacja w menu

#include <LiquidCrystal_I2C.h>
#include "Basic_Button.h"
#include "Basic_Encoder.h"
#include <Wire.h>
#include <avr/pgmspace.h>

const char empty_text[] PROGMEM = "";
const char MAKE_CALL_text[] PROGMEM = "1.MAKE CALL";
const char PHONE_BOOK_text[] PROGMEM = "2.PHONE BOOK";
const char PB_SEARCH_text[] PROGMEM = "1.SEARCH";
const char PB_ADD_text[] PROGMEM = "2.ADD";
const char PB_REMOVE_text[] PROGMEM = "3.REMOVE";
const char MESSAGES_text[] PROGMEM = "3.MESSAGES";
const char MSG_WRITE_text[] PROGMEM = "1.WRITE";
const char MSG_INBOX_text[] PROGMEM = "2.INBOX";
const char MSG_OUTBOX_text[] PROGMEM = "3.OUTBOX";
const char MSG_TEMPLATES_text[] PROGMEM = "4.TEMPLATES";
const char SETTINGS_text[] PROGMEM = "4.SETTINGS";
const char SET_TONES_text[] PROGMEM = "1.TONES";
const char SET_AUTO_ANSWER_text[] PROGMEM = "2.AUTO ANSWER";
const char SET_LANG_text[] PROGMEM = "3.LANGUAGE";
const char CLOCK_text[] PROGMEM = "5.CLOCK";
const char CLK_ALARM_text[] PROGMEM = "1.ALARM CLOCK";
const char CLK_SETTINGS_text[] PROGMEM = "2.CLOCK SETUP";
const char CLK_DATESET_text[] PROGMEM = "3.DATE SETUP";
const char CLK_STOPWATCH_text[] PROGMEM = "4.STOPWATCH";
const char CLK_COUNTDOWN_text[] PROGMEM = "5.COUNTDOWN";
const char CALCULATOR_text[] PROGMEM = "6.CALCULATOR";

const char MAKE_CALL_msg[] PROGMEM = "Making call";
const char PB_SEARCH_msg[] PROGMEM = "searching for the number";
const char PB_ADD_msg[] PROGMEM = "adding new phone number";
const char PB_REMOVE_msg[] PROGMEM = "removing phone number";
const char MSG_WRITE_msg[] PROGMEM = "creating new message";
const char MSG_INBOX_msg[] PROGMEM = "opening inbox";
const char MSG_OUTBOX_msg[] PROGMEM = "opening outbox";
const char MSG_TEMPLATES_msg[] PROGMEM = "choose message template";
const char SET_TONES_msg[] PROGMEM = "opening tones settins";
const char SET_AUTO_ANSWER_msg[] PROGMEM = "configure auto-answer feature";
const char SET_LANG_msg[] PROGMEM = "set language";
const char CLK_ALARM_msg[] PROGMEM = "set alarm";
const char CLK_SETTINGS_msg[] PROGMEM = "configure time";
const char CLK_DATESET_msg[] PROGMEM = "set date";
const char CLK_STOPWATCH_msg[] PROGMEM = "turn on stopwatch";
const char CLK_COUNTDOWN_msg[] PROGMEM = "turn on countdown";
const char CALCULATOR_msg[] PROGMEM = "opening calculator";

const char *const text_table[] PROGMEM = {
    empty_text,           //0
    MAKE_CALL_text,       //1
    PHONE_BOOK_text,      //2
    PB_SEARCH_text,       //3
    PB_ADD_text,          //4
    PB_REMOVE_text,       //5
    MESSAGES_text,        //6
    MSG_WRITE_text,       //7
    MSG_INBOX_text,       //8
    MSG_OUTBOX_text,      //9
    MSG_TEMPLATES_text,   //10
    SETTINGS_text,        //11
    SET_TONES_text,       //12
    SET_AUTO_ANSWER_text, //13
    SET_LANG_text,        //14
    CLOCK_text,           //15
    CLK_ALARM_text,       //16
    CLK_SETTINGS_text,    //17
    CLK_DATESET_text,     //18
    CLK_STOPWATCH_text,   //19
    CLK_COUNTDOWN_text,   //20
    CALCULATOR_text       //21
};
const char *const msg_table[] PROGMEM = {
    empty_text,          //0
    MAKE_CALL_msg,       //1
    PB_SEARCH_msg,       //2
    PB_ADD_msg,          //3
    PB_REMOVE_msg,       //4
    MSG_WRITE_msg,       //5
    MSG_INBOX_msg,       //6
    MSG_OUTBOX_msg,      //7
    MSG_TEMPLATES_msg,   //8
    SET_TONES_msg,       //9
    SET_AUTO_ANSWER_msg, //10
    SET_LANG_msg,        //11
    CLK_ALARM_msg,       //12
    CLK_SETTINGS_msg,    //13
    CLK_DATESET_msg,     //14
    CLK_STOPWATCH_msg,   //15
    CLK_COUNTDOWN_msg,   //16
    CALCULATOR_msg       //17
};

char buffer[64];

struct MenuNode
{
    MenuNode *prev_node;
    MenuNode *next_node;
    MenuNode *parent_node;
    MenuNode *child_node;
    byte text_index;
    byte msg_index;

    MenuNode(byte TextIndex, byte MessageIndex)
    {
        prev_node = nullptr;
        next_node = nullptr;
        parent_node = nullptr;
        child_node = nullptr;
        text_index = TextIndex;
        msg_index = MessageIndex;
    }
};

class Menu
{
private:
    MenuNode *master;

public:
    Menu(MenuNode *node)
    {
        master = node;
        master->prev_node = nullptr;
        master->child_node = nullptr;
    }

    Menu()
    {
        master = nullptr;
    }

    Menu &add(MenuNode &node)
    {
        if (master == nullptr)
        {
            master = &node;
        }
        else
        {
            MenuNode *current = master;
            MenuNode *prev;

            //go at the end
            while (current->next_node != nullptr)
            {
                prev = current;
                current = current->next_node;
            }

            current->next_node = &node;
            node.prev_node = current;
        }

        return *this;
    }

    void setParent(MenuNode &node)
    {
        if (master != nullptr)
        {
            node.child_node = master;
            MenuNode *current = master;
            //set parent in every node
            while (current->next_node != nullptr)
            {
                current->parent_node = &node;
                current = current->next_node;
            }

            //set last node
            current->parent_node = &node;
        }
    }
};

//arrow character
byte arrow[8] = {
    B00000,
    B00100,
    B01000,
    B11111,
    B01000,
    B00100,
    B00000,
};

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
Button greenButton(4), redButton(2);
Encoder encoder(A3, A2);

//hard-coded menu using MenuNodes

MenuNode MAKE_CALL(1, 1);
MenuNode PHONE_BOOK(2, 0);
MenuNode PB_SEARCH(3, 2);
MenuNode PB_ADD(4, 3);
MenuNode PB_REMOVE(5, 4);
MenuNode MESSAGES(6, 0);
MenuNode MSG_WRITE(7, 5);
MenuNode MSG_INBOX(8, 6);
MenuNode MSG_OUTBOX(9, 7);
MenuNode MSG_TEMPLATES(10, 8);
MenuNode SETTINGS(11, 0);
MenuNode SET_TONES(12, 9);
MenuNode SET_AUTO_ANSWER(13, 10);
MenuNode SET_LANG(14, 11);
MenuNode CLOCK(15, 0);
MenuNode CLK_ALARM(16, 12);
MenuNode CLK_SETTINGS(17, 13);
MenuNode CLK_DATESET(18, 14);
MenuNode CLK_STOPWATCH(19, 15);
MenuNode CLK_COUNTDOWN(20, 16);
MenuNode CALCULATOR(21, 17);

Menu MAIN_MENU;
Menu PB_MENU;
Menu MSG_MENU;
Menu SET_MENU;
Menu CLK_MENU;

MenuNode *current_node;
byte pointer_pos = 0;

void setup()
{
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.createChar(0, arrow);

    /////////////// setting up menu connections ////////////////////

    ///// submenus ///////

    //phone book submenu
    PB_MENU
        .add(PB_SEARCH)
        .add(PB_ADD)
        .add(PB_REMOVE)
        .setParent(PHONE_BOOK);

    //messages submenu
    MSG_MENU
        .add(MSG_WRITE)
        .add(MSG_INBOX)
        .add(MSG_OUTBOX)
        .add(MSG_TEMPLATES)
        .setParent(MESSAGES);

    //setting submenu
    SET_MENU
        .add(SET_TONES)
        .add(SET_AUTO_ANSWER)
        .add(SET_LANG)
        .setParent(SETTINGS);

    //clock submenu
    CLK_MENU
        .add(CLK_ALARM)
        .add(CLK_SETTINGS)
        .add(CLK_DATESET)
        .add(CLK_STOPWATCH)
        .add(CLK_COUNTDOWN)
        .setParent(CLOCK);

    ///// main menu ///////

    MAIN_MENU
        .add(MAKE_CALL)
        .add(PHONE_BOOK)
        .add(MESSAGES)
        .add(SETTINGS)
        .add(CLOCK)
        .add(CALCULATOR);

    ////////////////////////////////////////////////////////////////////

    current_node = &MAKE_CALL;

    updateDisplay();
    updatePointer();
}

void loop()
{
    onGreenButtonPressed();
    onRedButtonPressed();
    onEncoderRotated();
}

void onGreenButtonPressed()
{
    if (greenButton.wasReleased())
    {
        if (current_node->child_node != nullptr)
        {
            current_node = current_node->child_node;
            updateDisplay();
            pointer_pos = 0;
            updatePointer();
        }
        else
        {
            strcpy_P(buffer, (char *)pgm_read_word(&(msg_table[current_node->msg_index])));
            Serial.println(buffer);
        }
    }
}

void onRedButtonPressed()
{
    if (redButton.wasReleased())
    {
        if (current_node->parent_node != nullptr)
        {
            current_node = current_node->parent_node;
            updateDisplay();
            pointer_pos = 0;
            updatePointer();
        }
    }
}

void onEncoderRotated()
{
    byte rotation = encoder.readRotation();

    if (rotation == ENCODER_ROTATED_CW)
    {
        if (current_node->next_node != nullptr)
        {
            pointer_pos++;
            current_node = current_node->next_node;
        }
    }
    else if (rotation == ENCODER_ROTATED_CCW)
    {
        if (current_node->prev_node != nullptr)
        {
            pointer_pos--;
            current_node = current_node->prev_node;
        }
    }

    updatePointer();
}

void updateDisplay()
{
    lcd.clear();
    strcpy_P(buffer, (char *)pgm_read_word(&(text_table[current_node->text_index])));
    lcd.print(buffer);

    if (current_node->next_node != nullptr)
    {
        strcpy_P(buffer, (char *)pgm_read_word(&(text_table[current_node->next_node->text_index])));
        lcd.setCursor(0, 1);
        lcd.print(buffer);
    }
}

void updatePointer()
{
    if (pointer_pos == 0)
    {
        lcd.setCursor(15, 1);
        lcd.print(" ");

        lcd.setCursor(15, 0);
        lcd.write(byte(0));
    }
    else if (pointer_pos == 1)
    {
        lcd.setCursor(15, 0);
        lcd.print(" ");

        lcd.setCursor(15, 1);
        lcd.write(byte(0));
    }
    else
    {
        updateDisplay();
        lcd.setCursor(15, 0);
        lcd.write(byte(0));
        pointer_pos = 0;
    }
}
