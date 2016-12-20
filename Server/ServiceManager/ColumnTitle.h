#if !defined ___COLUMN_TITLE_H
#define ___COLUMN_TITLE_H

#include "StatusConst.h"


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#ifdef _USE_ENGLISH
static const LPTSTR ColumnTitle[]={ "Access Address",    //  0
			  				        "Available ID for registration",       //  1
							        "Log Out",         //  2
							        "Used Time",         //  3
									"Password",         //  4
									"Beginning Date",         //  5
									"Ending Date",         //  6
									"Charge Code",         //  7
									"Ending Code",         //  8
									"Deposit Bank",         //  9
									"Used Period",         // 10
									"Date of receipt",         // 11
									"Reason of suspension",         // 12
									"Registered ID",         // 13
									"User ID",         // 14
									"Depositor",           // 15
									"Login",           // 16
									"Amount of deposit",           // 17
									"Operator",           // 18
									"Date of registration",           // 19
									"Date of termination",           // 20
									"Service",           // 21
 			 						"Number",             // 22
									"Type",             // 23
									"Variety",             // 24
									"Process",             // 25
									"Note",             // 26
									"Firm Name",             // 27
									"Registered Business Number",  // 28
									"Charge",           // 29
									"VAT" };         // 30
#else
static const LPTSTR ColumnTitle[]={ "���� ��巹��",    //  0
			  				        "��ϰ���ID",       //  1
							        "�α׾ƿ�",         //  2
							        "�̿�ð�",         //  3
									"��й�ȣ",         //  4
									"��������",         //  5
									"��������",         //  6
									"�����ڵ�",         //  7
									"�����ڵ�",         //  8
									"�Ա�����",         //  9
									"���Ⱓ",         // 10
									"�Ա�����",         // 11
									"��������",         // 12
									"��ϵ�ID",         // 13
									"�����ID",         // 14
									"�Ա���",           // 15
									"�α���",           // 16
									"�Աݾ�",           // 17
									"�۾���",           // 18
									"�����",           // 19
									"������",           // 20
									"����",           // 21
 			 						"��ȣ",             // 22
									"Ÿ��",             // 23
									"����",             // 24
									"ó��",             // 25
									"���",             // 26
									"��ȣ",             // 27
									"����� ��Ϲ�ȣ",  // 28
									"����",           // 29
									"�ΰ���" };         // 30
#endif

static const COLUMNINFO LogColumn[] = { { 70, ColumnTitle[21]}     // ����
                                       ,{ 90, ColumnTitle[14]}     // �����ID
									   ,{120, ColumnTitle[16]}     // �α���
									   ,{120, ColumnTitle[2] }     // �α׾ƿ�
									   ,{ 60, ColumnTitle[3] }     // �̿�ð�
									   ,{ 60, ColumnTitle[7] }     // �����ڵ�
									   ,{ 60, ColumnTitle[8] }     // �����ڵ�
									   ,{100, ColumnTitle[0] } };  // ���� ��巹��

static const COLUMNINFO SerialColumn[] = { { 50, ColumnTitle[22]}     // ��ȣ
                                          ,{ 80, ColumnTitle[23]}     // Ÿ��
										  ,{ 90, ColumnTitle[4] }     // ��й�ȣ
										  ,{110, ColumnTitle[5] }     // ��������
										  ,{110, ColumnTitle[6] }     // ��������
										  ,{ 80, ColumnTitle[1] }     // ��ϰ���ID
										  ,{ 80, ColumnTitle[13]}     // ��ϵ�ID
										  ,{ 60, ColumnTitle[10]} };  // ���Ⱓ

static const COLUMNINFO BankColumn[] = { { 60, ColumnTitle[22]}     // ��ȣ
                                        ,{120, ColumnTitle[11]}     // �Ա�����
										,{ 90, ColumnTitle[15]}     // �Ա���
										,{ 90, ColumnTitle[9] }     // �Ա�����
										,{ 90, ColumnTitle[17]}     // �Աݾ�
										,{ 90, ColumnTitle[24]}     // ����
										,{ 90, ColumnTitle[25]}     // ó��
										,{ 90, ColumnTitle[18]}     // �۾���
										,{200, ColumnTitle[26]} };  // ���

static const COLUMNINFO BadIDColumn[] = { { 90, ColumnTitle[14]}     // �����ID
								         ,{120, ColumnTitle[19]}     // �����
								         ,{120, ColumnTitle[20]}     // ������
								         ,{ 90, ColumnTitle[18]}     // �۾���
								         ,{200, ColumnTitle[12]} };  // ��������

static const COLUMNINFO ClientPayColumn[] = { {180, ColumnTitle[27]}     // ��ȣ
											 ,{100, ColumnTitle[28]}     // ����� ��Ϲ�ȣ
											 ,{ 90, ColumnTitle[3] }     // �̿�ð�
											 ,{200, ColumnTitle[10]}     // ���Ⱓ
											 ,{ 90, ColumnTitle[29]}     // ����
											 ,{ 90, ColumnTitle[30]} };  // �ΰ���

static const COLINFO __ColInfo[] = { { (LPCOLUMNINFO) LogColumn,       8}
						            ,{ (LPCOLUMNINFO) SerialColumn,    8}
						            ,{ (LPCOLUMNINFO) BankColumn,      9}
						            ,{ (LPCOLUMNINFO) BadIDColumn,     5}
									,{ (LPCOLUMNINFO) ClientPayColumn, 6} };


LPCOLINFO ColInfo = (LPCOLINFO) __ColInfo;

#endif // !defined ___COLUMN_TITLE_H