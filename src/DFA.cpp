#include "DFA.h"
#include <string>
#include <stack>
#include <iostream>
using namespace std;
template <class T>
class LinkStack;

//定义链式栈结点类
template <class T>
class StackNode
{
	friend class LinkStack<T>;
private:
	T data;
	StackNode<T> *next;
	StackNode(T item = 0, StackNode<T> *p = NULL)
	{
		date = item;
		next = p;
	}
};

// 定义链式栈类
template <class T>
class LinkStack
{
private:
	StackNode<T> *top;
public:
	LinkStack();
	~LinkStack();
	bool IsEmpty();
	int Length();
	void Push(const T &item);
	T Pop();
	T GetTop();
	void Clear();
};
//定义构造函数
template <class T>
LinkStack<T>::LinkStack()
{
	top = NULL;
}

//定义析构函数
template <class T>
LinkStack<T>::~LinkStack()
{
	Clear();
}
//判断栈是否为空
template <class T>
bool LinkStack<T>::IsEmpty()
{
	return(!top);
}
//栈的长度
template <class T>
int LinkStack<T>::Length()
{
	StackNode<T> *temp = new StackNode<T>();
	temp = top;
	int length = 0;
	while (temp)
	{
		temp = temp->next;
		length++;
	}
	return length;
}
//压栈
template <class T>
void LinkStack<T>::Push(const T &item)
{
	top = new StackNode<T>(item, top);
}
// 抽出数据(出栈)
template <class T>
T LinkStack<T>::Pop()
{
	if (!IsEmpty())
	{
		StackNode<T> *temp = top;
		top = top->next;
		T value = temp->data;
		delete temp;
		return value;
	}
	else
	{
		cout << "Stack is Empty" << endl;
		exit(1);
	}
}
//获取栈顶数据
template<class T>
T LinkStack<T>::GetTop()
{
	if (!IsEmpty())
		return top->data;
	else
	{
		cout << "The Stack is Empty" << endl;
		exit(1);
	}
}
//清空栈
template<class T>
void LinkStack<T>::Clear()
{
	StackNode<T> *temp = new StackNode<T>();
	while (top)
	{
		temp = top;
		top = top->next; 
		delete temp;
	}
}
/*
*/
//获取正则输入
void DFA::InputRegex()
{
	cout << " 请输入正则表达式" << endl;
	cin >> Regex;
}
//加入连结点
void DFA::InsertNode()
{
	int i = 0,j, len = strlen(Regex);
	int length = len;
	while (len--&&i<=length-1)
	{
		if (((Regex[i] != '(' && Regex[i] != '.' && Regex[i] != '|')
			|| Regex[i] == ')'
			|| Regex[i] == '*')
			&& (Regex[i + 1] != ')' && Regex[i + 1] != '.' && Regex[i + 1] != '|' && Regex[i + 1] != '*'))
		{
			for (j = len; j>i + 1; j--)
			{
				Regex[j] = Regex[j - 1];
			}
			Regex[i + 1] = '.';
			len++;
			Regex[len] = '\0';
			i++;
		}
		i++;
	}
	cout << "\n第一步: 加入连结点\n"
		<< Regex << "\n"
		<< "字符串长度: " << len
		<< "\n\n------------------------" << endl;
	cout << "\n第一步：加入连结点\n" << Regex << endl;
	cout << "字符串长度：" <<length<<endl;
}
//判断运算符优先级
int DFA::Precedence(char symbol)
{
	int priority;
	switch (symbol)
	{
	case '|':priority = 1; break;
	case '.':priority = 2; break;
	case '*':priority = 3; break;
	default:priority = 0; break;
	}
	return priority;
}
//将正则式转为逆波兰式
void DFA::RegextoPost()
{
	int i = 0, j = 0;
	char ch, cl;
	strcpy(RegexPost, "\0");
	LinkStack<char> *ls = new LinkStack<char>();
	ls->Clear();
	ls->Push('#');
	ch = Regex[i];
	while (ch != '\0')
	{
		if (ch == '(')
		{
			ls->Push(ch);
			ch = Regex[++i];
		}
		else if (ch = ')')
		{
			while (ls->GetTop() != '(')
				RegexPost[j++] = ls->Pop();
			ls->Pop();
			ch = Regex[++i];
		}
		else if ((ch == '|') || (ch = '*') || (ch == '.'))
		{
			cl = ls->GetTop();
			while (Precedence(cl) >= Precedence(ch))
			{
				RegexPost[j++] = cl;
				ls->Pop();
				cl = ls->GetTop();
			}
			ls->Push(ch);
			ch = Regex[++i];
		}
		else{
			RegexPost[j++] = ch;
			ch = Regex[++i];
		}
	}
	ch = ls->Pop();
	while ((ch == '|') || (ch == '*') || (ch == '.'))
	{
		RegexPost[j++] = ch;
		ch = ls->Pop();
	}
	RegexPost[j] = '\0';
	ls->Clear();
	cout << "转为后缀式" << endl;
	cout << RegexPost << endl;
	cout << "字符串长度：" << strlen(RegexPost) << endl;
}
//扫描逆波兰式中除运算符以外的字符的数目
void DFA::GetEdgeNumber()
{
	int i = 0, j;
	EdgeNumber = 0;
	while (RegexPost[i] != '\0')
	{
		if (RegexPost[i] == '.' || RegexPost[i] == '|' || RegexPost[i] == '*')
		{
			i++;
			continue;
		}
		for (j = 0; j < EdgeNumber; j++)
		{
			if (RegexPost[i] == Edge[j])
				break;
		}
		if (j == EdgeNumber)
		{
			Edge[EdgeNumber] = RegexPost[i];
			EdgeNumber++;
		}
		i++;
	}
	Edge[EdgeNumber] == '\0';
	cout << "\n第三步: 获取字符集\n";
	for (i = 0; i<EdgeNumber; i++)
	{
		cout << Edge[i] << ' ';
	}
	cout << "\n字符个数: " << EdgeNumber
		<< "\n\n------------------------" << endl;
}
//用Thompson构造法构造NFA
void DFA::Thompson()
{ 
	int i, j;
	char ch;
	int s1, s2;
	LinkStack<int >*States = new LinkStack<int >();
	States->Clear();
	if (strlen(RegexPost) < 1)
	{
		cout << "No Regex Expression Find" << endl;
		exit(1);
	}
	
}