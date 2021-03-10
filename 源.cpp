#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#define SIZE 1024
#define keyword_num 44
//C���Թؼ���
std::string keywords[44] = { "auto","break","case","char","const","continue",
                          "default","do","double","else","enum","extern",
                          "float","for","goto","if","int","long","register",
                          "return","short","signed","sizeof","static",
                          "struct","switch","typedef","union","unsigned",
                          "void","volatile","while","inline","restrict",
                          "_Bool","_Complex","_Imaginary",//C99�����Ĺؼ���
                          "_Alignas","_Alignof","_Atomic","_Static_assert",
                          "_Noreturn","_Thread_local","_Generic" };//C11�����Ĺؼ���

int state;//��ǰ״ָ̬ʾ
char C;//��ŵ�ǰ������ַ�
int iskey = -1;
/*��ʾʶ����ĵ����Ƿ�Ϊ�û��Զ����ʶ�����ǹؼ��֣�
  ������û��Զ����ʶ����Ϊ-1�����ǹؼ������ǹؼ��ֵļǺ�*/
std::string token;//�ַ����飬��ŵ�ǰ����ʶ��ĵ����ַ���
char* lexemebegin;//�ַ�ָ�룬ָ�����뻺�����е�ǰ���ʵĿ�ʼλ��
int forward = -1;//��ǰָ��
char buffer[SIZE];//���뻺����
int lines = 1;//�ļ�������
int chars = 0;//�ļ������ַ���
struct symbol {
    std::string attribute;//����
    std::string name;//����
    std::string token;//�Ǻ�
    std::string category;//���
    int count = 0;//����
};//���ű�
struct count {
    int count_keyword = 0;//�ؼ���
    int count_id = 0;//��ʶ��
    int count_num = 0;//����
    int count_string = 0;//�ַ���
    int count_assignment_operator = 0;//��ֵ�����
    int count_bit_operator = 0;//λ�����
    int count_arithmetic_operator = 0;//���������
    int count_logical_operator = 0;//�߼������
    int count_char = 0;//�ַ�
    int count_relational_operator = 0;//��ϵ�����
    int count_other_symbol = 0;//��������
};//ͳ�Ʊ�

count c;
std::vector<symbol> table;
/*ÿ����һ�Σ�������ǰָ��forward��ָʾ�������뻺�����ж�һ���ַ���
  �������������C�У�Ȼ���ƶ�forwardʹָ֮����һ���ַ�*/
void get_char() {
    forward++;
    C = buffer[forward];
    if (C == '\n')
        lines++;//ͳ������
    if (C != '\n' && C != '\t' && C != ' ')
        chars++;//ͳ���ַ���
}
/*ÿ�ε���ʱ�����C�е��ַ��Ƿ�Ϊ�ո����ǿո񣬾ͷ������ù���get_char��
  ֱ��C�н���һ���ǿ��ַ�Ϊֹ*/
void get_nbc() {
    if (C == ' ') {
        get_char();
    }
}
//ÿ�ε���ʱ����C�е��ַ�������token�е��ַ�������
void cat() {
    token.push_back(C);
}
//�ж�C�е��ַ��Ƿ�Ϊ��ĸ�������ַ��򷵻�true�����򷵻�false
bool isLetter() {
    if ((C <= 'z' && C >= 'a') || (C <= 'Z' && C >= 'A') || C == '_')
        return true;
    else
        return false;
}
//�ж�C�����е��ַ��Ƿ�Ϊ���֣����������򷵻�true�����򷵻�false
bool isDigit() {
    if (C <= '9' && C >= '0')
        return true;
    else
        return false;
}
//��ǰָ��forw����һ���ַ�
void retract() {
    forward--;
}
/*����token�еĵ��ʲ�ؼ��ֱ���token�еĵ����ǹؼ��֣�
  �򷵻�1�����򷵻�-1*/
int reserve() {
    for (int i = 0; i < 44; i++) {
        if (token == keywords[i])
            return 1;//�ǹؼ��־ͷ���1
    }
    return -1;//���ǹؼ��־ͷ���-1

}
//��token�е��ַ���ת��������
int SToI() {
    return atoi(token.c_str());
}
//��token�е��ַ���ת���ɸ�����
float SToF() {
    return atof(token.c_str());
}
//��ʶ��������üǺŲ�����ű�
int table_insert(std::string t, std::string a, std::string c) {
    int i = 0;
    for (i = 0; i < table.size(); i++) {
        if (token == table[i].name) {
            table[i].count++;
            return 0;
        }
    }
    symbol s;
    s.name = token;
    s.token = t;
    s.attribute = a;
    s.count = 1;
    s.category = c;
    table.push_back(s);
    return 0;

}
std::string find(std::string name) {
    int i = 0;
    for (; i < table.size(); i++) {
        if (table[i].name == name) {
            return std::to_string(i);
        }
    }
    return std::to_string(i);
}
//�Է��ֵĴ��������Ӧ�Ĵ���
void error() {
    std::cout << "There is an error in line " << lines << std::endl;
    state = 0;

}

void analyse(std::ifstream& fs) {
    //��ȡ�ļ�����
    fs.read(buffer, SIZE - 1);

    if (fs.gcount() < SIZE - 1) {
        buffer[fs.gcount()] = EOF;//�����뻺�������������־
    }
    state = 0;//��ʼ״̬
    int j = 0;
    while (C != EOF) {
        switch (state) {
        case 0:
            token.clear();
            get_char();
            get_nbc();
            if (isLetter()) {
                state = 1;
                break;
            }
            if (isDigit()) {
                state = 2;
                break;
            }
            switch (C) {
            case '<':
                state = 8;
                break;
            case '>':
                state = 9;
                break;
            case ':':
                state = 10;
                break;
            case '/':
                state = 11;
                break;
            case '=':
                state = 14;
                break;
            case '+':
                state = 15;
                break;
            case '-':
                state = 16;
                break;
            case '(':
                state = 0;
                cat();
                table_insert("(", "-", "ohter symbol");
                break;
            case ')':
                state = 0;
                cat();
                table_insert(")", "-", "ohter symbol");
                break;
            case ';':
                state = 0;
                cat();
                table_insert(";", "-", "ohter symbol");
                break;
            case '?':
                state = 0;
                cat();
                table_insert("?", "-", "conditional operator");
                break;
            case '%':
                state = 0;
                cat();
                table_insert("%", "-", "arithmetic operator");
                break;
            case '{':
                state = 0;
                cat();
                table_insert("{", "-", "ohter symbol");
                break;
            case '}':
                state = 0;
                cat();
                table_insert("}", "-", "ohter symbol");
                break;
            case '[':
                state = 0;
                cat();
                table_insert("[", "-", "ohter symbol");
                break;
            case ']':
                state = 0;
                cat();
                table_insert("]", "-", "ohter symbol");
                break;
            case '|':
                state = 18;
                break;
            case '&':
                state = 19;
                break;
            case '^':
                state = 0;
                cat();
                table_insert("^", "-", "bit operator");
                break;
            case '*':
                state = 20;
                break;
            case '"':
                state = 21;
                break;
            case '.':
                state = 0;
                break;
            case '~':
                state = 0;
                cat();
                table_insert("~", "-", "bit operator");
                break;
            case ',':
                state = 0;
                cat();
                table_insert(",", "-", "ohter symbol");
                break;
            case '#':
                state = 0;
                cat();
                table_insert("#", "-", "ohter symbol");
                break;
            case '\'':
                state = 22;
                break;
            case ' ':
                break;
            case '\n':
                break;
            case '\t':
                break;
            default:
                state = 13;
                break;
            }
            break;
        case 1://��ʶ��״̬
            cat();
            get_char();
            if (isLetter() || isDigit())
                state = 1;
            else {
                retract();
                state = 0;
                if (reserve() == 1) {
                    table_insert(token, "-", "keyword");
                }
                else {
                    table_insert("id", find(token), "id");
                }
            }
            break;
        case 2://����״̬
            cat();
            get_char();
            if (isDigit())
                state = 2;
            else if (C == 'e' || C == 'E')
                state = 5;
            else if (C == '.')
                state = 3;
            else {
                retract();
                state = 0;
                table_insert("num", token, "integer");
            }
            break;
        case 3://С����״̬
            cat();
            get_char();
            if (isDigit())
                state = 4;
            else {
                state = 13;
            }
            break;
        case 4://С��״̬
            cat();
            get_char();
            if (isDigit())
                state = 4;
            else if (C == 'e' || C == 'E')
                state = 5;
            else {
                retract();
                state = 0;
                table_insert("num", token, "float");
            }
            break;
        case 5://ָ��״̬
            cat();
            get_char();
            if (isDigit())
                state = 7;
            else if (C == '+' || C == '-')
                state = 6;
            else {
                retract();
                state = 13;
            }
            break;
        case 6:
            cat();
            get_char();
            if (isDigit())
                state = 7;
            else {
                retract();
                state = 13;
            }
            break;
        case 7:
            cat();
            get_char();
            if (isDigit())
                state = 7;
            else {
                retract();
                state = 0;
                table_insert("num", token, "exponent");
            }
            break;
        case 8://'<'״̬
            cat();
            get_char();
            switch (C) {
            case '=':
                cat();
                state = 0;
                table_insert("relop", "LE", "relational operator");
                break;
            case '>':
                cat();
                state = 0;
                table_insert("relop", "NE", "relational operator");
                break;
            case '<':
                cat();
                state = 0;
                table_insert(token, "-", "bit operator");
                break;
            default:
                retract();
                state = 0;
                table_insert("relop", "LT", "relational operator");
                break;
            }
            break;
        case 9://'>'״̬
            cat();
            get_char();
            if (C == '=') {
                cat();
                state = 0;
                table_insert("relop", "GE", "relational operator");
            }
            else if (C == '>') {
                cat();
                state = 0;
                table_insert(token, "-", "bit operator");
            }
            else {
                retract();
                state = 0;
                table_insert("relop", "GT", "relational operator");
            }
            break;
        case 10://':'״̬
            cat();
            get_char();
            if (C == '=') {
                cat();
                state = 0;
                table_insert("assign-op", "-", "assign-op");
            }
            else {
                retract();
                state = 0;
                table_insert(token, "-", "ohter symbol");
            }
            break;
        case 11://'/'״̬
            cat();
            get_char();
            if (C == '*')
                state = 12;
            else if (C == '/')
                state = 17;
            else if (C == '=') {
                cat();
                state = 0;
                table_insert(token, "-", "assignment operator");
            }

            else {
                retract();
                state = 0;
                table_insert(token, "-", "arithmetic operator");
            }
            break;
        case 12://����/*ע��״̬��־
            get_char();
            while (C != '*') {
                get_char();
                chars--;
            }
            get_char();
            if (C == '/') {
                token.clear();
                chars = chars - 4;
                state = 0;
            }
            else
                state = 12;
            break;
        case 13:
            error();
            state = 0;
            break;
        case 14://'='״̬
            cat();
            get_char();
            if (C == '=') {
                cat();
                state = 0;
                table_insert(token, "-", "relational operator");
            }
            else {
                retract();
                state = 0;
                table_insert("relop", "EQ", "assignment operator");
            }
            break;
        case 15://'+'״̬
            cat();
            get_char();
            if (C == '+') {
                cat();
                state = 0;
                table_insert(token, "-", "arithmetic operator");
            }
            else if (C == '=') {
                cat();
                state = 0;
                table_insert(token, "-", "assignment operator");
            }
            else {
                retract();
                state = 0;
                table_insert(token, "-", "arithmetic operator");
            }
            break;
        case 16://'-'״̬
            cat();
            get_char();
            if (C == '-') {
                cat();
                state = 0;
                table_insert(token, "-", "arithmetic operator");
            }
            else if (C == '=') {
                cat();
                state = 0;
                table_insert(token, "-", "assignment operator");
            }
            else {
                retract();
                state = 0;
                table_insert(token, "-", "arithmetic operator");
            }
            break;
        case 17://����//ע��״̬��־
            get_char();
            while (C != '\n') {
                get_char();
                chars--;
            }
            token.clear();
            chars = chars - 2;
            state = 0;
            break;
        case 18: //����'|'״̬��־
            cat();
            get_char();
            if (C == '|') {
                cat();
                state = 0;
                table_insert(token, "-", "logical operator");
            }
            else {
                retract();
                state = 0;
                table_insert(token, "-", "bit operator");
            }
            break;
        case 19://����'&'״̬��־
            cat();
            get_char();
            if (C == '&') {
                cat();
                state = 0;
                table_insert(token, "-", "logical operator");
            }
            else {
                retract();
                state = 0;
                table_insert(token, "-", "bit operator");
            }
            break;
        case 20://����'*'״̬��־
            cat();
            get_char();
            if (C == '=') {
                cat();
                state = 0;
                table_insert(token, "-", "assignment operator");
            }
            else {
                retract();
                state = 0;
                table_insert(token, "-", "arithmetic operator");
            }
            break;
        case 21://����'"'״̬��־
            cat();
            get_char();
            while (C != '"') {
                cat();
                get_char();
            }
            cat();
            state = 0;
            table_insert("literal", token, "string");
            break;

        case 22://����'''״̬��־
            cat();
            get_char();
            j = 0;

            while (C != '\'') {
                cat();
                get_char();
                j++;

                if (C == '\n') {
                    state = 13;
                    break;
                }
            }
            if (j > 1) {
                error();
                state = 0;
                break;
            }
            cat();
            state = 0;
            table_insert("char", token, "char");
            break;
        default:error(); state = 0; break;
        }
    }
    fs.close();
}
void output() {

    std::cout << "name \t\ttimes\t\ttoken\t\tattribute\t\tcategory\n";
    for (int i = 0; i < table.size(); i++) {
        if (table[i].category == "keyword")
            c.count_keyword++;
        if (table[i].category == "id")
            c.count_id++;
        if (table[i].token == "num")
            c.count_num++;
        if (table[i].category == "string")
            c.count_string++;
        if (table[i].category == "relational operator")
            c.count_relational_operator++;
        if (table[i].category == "relational operator")
            c.count_relational_operator++;
        if (table[i].category == "assignment operator")
            c.count_assignment_operator++;
        if (table[i].category == "bit operator")
            c.count_bit_operator++;
        if (table[i].category == "arithmetic operator")
            c.count_arithmetic_operator++;
        if (table[i].category == "logical operator")
            c.count_logical_operator++;
        if (table[i].category == "other symbol")
            c.count_other_symbol++;
        if (table[i].category == "char")
            c.count_char++;
        std::cout << table[i].name << "\t\t" << table[i].count << "\t\t" << table[i].token <<
            "\t\t" << table[i].attribute << "\t\t\t" << table[i].category << std::endl;
    }

    if (c.count_keyword > 0)
        std::cout << "count_keyword:" << c.count_keyword << std::endl;
    if (c.count_id > 0)
        std::cout << "count_id:" << c.count_id << std::endl;
    if (c.count_num > 0)
        std::cout << "count_num:" << c.count_num << std::endl;
    if (c.count_string > 0)
        std::cout << "count_string:" << c.count_string << std::endl;
    if (c.count_char > 0)
        std::cout << "count_char:" << c.count_char << std::endl;
    if (c.count_relational_operator > 0)
        std::cout << "count_relational_operator:" << c.count_relational_operator << std::endl;
    if (c.count_assignment_operator > 0)
        std::cout << "count_assignment_operator:" << c.count_assignment_operator << std::endl;
    if (c.count_bit_operator > 0)
        std::cout << "count_bit_operator:" << c.count_bit_operator << std::endl;
    if (c.count_arithmetic_operator > 0)
        std::cout << "count_arithmetic_operator:" << c.count_arithmetic_operator << std::endl;
    if (c.count_logical_operator > 0)
        std::cout << "count_logical_operator:" << c.count_logical_operator << std::endl;
    std::cout << "line:" << lines << std::endl << "chars:" << chars;


}
int main() {
    std::ifstream fs;
    fs.open("t.txt", std::ios::in);
    if (fs.is_open() == false)
        exit(0);
    analyse(fs);
    output();
    fs.close();
    return 0;
}