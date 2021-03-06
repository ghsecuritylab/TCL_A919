
#ifndef NETWORKCONFIG_H
#define NETWORKCONFIG_H

#include <QObject>
#include "ui_networkconfig.h"

class WpaGui;

class NetworkConfig : public QDialog, public Ui::NetworkConfig
{
	Q_OBJECT

public:
	NetworkConfig(QWidget *parent = 0, const char *name = 0,
		      bool modal = false, Qt::WFlags fl = 0);
	~NetworkConfig();

	virtual void paramsFromScanResults(Q3ListViewItem *sel);
	virtual void setWpaGui(WpaGui *_wpagui);
	virtual int setNetworkParam(int id, const char *field,
				    const char *value, bool quote);
	virtual void paramsFromConfig(int network_id);
	virtual void newNetwork();

public slots:
	virtual void authChanged(int sel);
	virtual void addNetwork();
	virtual void encrChanged(const QString &sel);
	virtual void writeWepKey(int network_id, QLineEdit *edit, int id);
	virtual void removeNetwork();

protected slots:
	virtual void languageChange();

private:
	WpaGui *wpagui;
	int edit_network_id;
	bool new_network;

	virtual void wepEnabled(bool enabled);
	virtual void getEapCapa();
};

#endif /* NETWORKCONFIG_H */
