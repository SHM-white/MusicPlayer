﻿#include "BasicWidget.h"


BasicWidget::BasicWidget(QWidget *parent)
	: QWidget(parent)
{
	//setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
	//setWindowFlags( Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);//背景半透明属性设置   //窗口透明 
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

#ifdef _WIN32

	if (m_enableDWM){
		HWND hWnd = HWND(this->winId());
		HMODULE hUser = GetModuleHandle(TEXT("user32.dll"));
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

QColor BasicWidget::backgroundColor() const
{
    return m_backgroundColor;
}

void BasicWidget::setBackgroundColor(const QColor &newBackgroundColor)
{
    if (m_backgroundColor == newBackgroundColor)
        return;
    m_backgroundColor = newBackgroundColor;
    emit backgroundColorChanged();
}
