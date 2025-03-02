#include "BasicWidget.h"


BasicWidget::BasicWidget(QWidget *parent)
	: QWidget(parent)
{
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
	//setWindowFlags( Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);//背景半透明属性设置   //窗口透明 

#ifdef _WIN32

	HWND hWnd = HWND(this->winId());
	HMODULE hUser = GetModuleHandle("user32.dll");
	if (hUser)
	{


		pfnSetWindowCompositionAttribute setWindowCompositionAttribute = (pfnSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
		if (setWindowCompositionAttribute)
		{
			ACCENT_POLICY accent = { ACCENT_ENABLE_BLURBEHIND, 0, 0, 0 };
			WINDOWCOMPOSITIONATTRIBDATA data;
			data.Attrib = WCA_ACCENT_POLICY;
			data.pvData = &accent;
			data.cbData = sizeof(accent);
			setWindowCompositionAttribute(hWnd, &data);
		}
	}
#endif // _WIN32


	m_backgroundColor = QColor(255, 255, 255, 100);

}

BasicWidget::~BasicWidget()
{}

void BasicWidget::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);
	painter.fillRect(this->rect(), m_backgroundColor);
}
