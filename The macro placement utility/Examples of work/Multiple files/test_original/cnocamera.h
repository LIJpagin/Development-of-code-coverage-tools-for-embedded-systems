#ifndef CNOCAMERA_H
#define CNOCAMERA_H

#include "../nosystem.h"
#include "../camera/ccamera_base.h"

// Класс реализующий работу с Altera
class CNoCamera : public CCameraBase
{
public:
	static const char* Name[];
	static const MODULE_ID id = idCameraBlank;
	virtual MODULE_ID GetID()   {  return id;   }
	virtual void LoadLanguage(unsigned int lang);
	virtual void DrawForeground(){}
	virtual void InitLanguageVariables(){}
	virtual void InitSetupMenu(){}
	virtual void SetupMenuClick(int, SETUP_ACTION){}
	virtual bool GetWarning(const char* &buff, WARNING_PRIORITY &prior, COLOR &color)	{ return false; }
	virtual void Init();
};


#endif /* CNOCOURSE_H */
