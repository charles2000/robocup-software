//  put "#pragma once" at the top of header files to protect against being included multiple times
#pragma once

#include <QtWidgets>
#include <QPainter>
#include <QRect>

class BatteryWidget : public QWidget {
public:
	BatteryWidget ( QWidget * parent = 0, Qt::WindowFlags f = 0 ):QWidget(parent, f) {};
	virtual void paintEvent ( QPaintEvent * event )	{
		QRect rect = event->rect();
    		draw(rect);
	}


	void draw(QRect &rect)
	{
	    int value =10;
	    QPainter painter(this);
	    painter.setRenderHint(QPainter::Antialiasing);
	    painter.setPen(Qt::black);
	    //painter.drawText(rect, Qt::AlignCenter,
		              //"Data");
	    painter.drawText(rect, Qt::AlignCenter,QString("%1").arg(value));
	    painter.drawRect(rect);
	    painter.drawRect(rect.x(),rect.y(),(rect.width()*11)/10,rect.height());// background rectangle
	    painter.fillRect(rect.x(),rect.y(),((rect.width()*11)/10)*value,rect.height(),Qt::red);// rectangle proportionnal to battery
    	    painter.fillRect(rect.x()+(rect.width()*11)/10,rect.y()/2,rect.width()/10,rect.height()/5,Qt::black);// rectangle to look like a battery.
	    
	}
	

		
};
