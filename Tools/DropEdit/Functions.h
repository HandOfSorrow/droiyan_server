#ifndef Functions_h__
#define Functions_h__

#include <QtCore>

#include "NpcItem.h"

namespace DropEdit
{
	//���ļ���ȡ������Ʒ��Ϣ
	bool getDropInfoFromINIFile(const QString& filepath, drop_info& dropItem);
}
#endif // Functions_h__