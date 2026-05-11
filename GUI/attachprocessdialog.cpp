#include "attachprocessdialog.h"
#include <QtWinExtras/QtWin>
#include <QAbstractItemView>

extern const char* SELECT_PROCESS;
extern const char* ATTACH_INFO;

namespace
{
	constexpr int ProcessIdRole = Qt::UserRole + 1;
	constexpr int ProcessPathRole = Qt::UserRole + 2;
	constexpr int ProcessNameRole = Qt::UserRole + 3;
}

AttachProcessDialog::AttachProcessDialog(QWidget* parent, const std::vector<ProcessInfo>& processes) :
	QDialog(parent, Qt::WindowCloseButtonHint),
	model(this)
{
	ui.setupUi(this);
	setWindowTitle(SELECT_PROCESS);
	ui.label->setText(ATTACH_INFO);
	ui.processEdit->setPlaceholderText(QStringLiteral("Type to filter or enter a PID"));
	ui.processList->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.processList->setUniformItemSizes(true);
	ui.processList->setIconSize(QSize(18, 18));
	ui.processList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.processList->setModel(&model);

	QPixmap transparent(100, 100);
	transparent.fill(QColor::fromRgba(0));
	for (const auto& process : processes)
	{
		QString displayText = QString("%1: %2").arg(process.id).arg(process.fileName);
		auto item = new QStandardItem(process.icon ? QIcon(QtWin::fromHICON(process.icon)) : transparent, displayText);
		item->setData(QString::number(process.id), ProcessIdRole);
		item->setData(process.path, ProcessPathRole);
		item->setData(process.fileName, ProcessNameRole);
		item->setToolTip(process.path);
		item->setEditable(false);
		model.appendRow(item);
	}

	connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(ui.buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	connect(ui.processList, &QListView::clicked, [this](QModelIndex index) { ui.processEdit->setText(model.item(index.row())->text()); });
	connect(ui.processList, &QListView::doubleClicked, this, &QDialog::accept);
	connect(ui.processEdit, &QLineEdit::textEdited, [this](QString process)
	{
		for (int i = 0; i < model.rowCount(); ++i)
		{
			auto item = model.item(i);
			bool matches =
				item->text().contains(process, Qt::CaseInsensitive) ||
				item->data(ProcessIdRole).toString().contains(process, Qt::CaseInsensitive) ||
				item->data(ProcessNameRole).toString().contains(process, Qt::CaseInsensitive) ||
				item->data(ProcessPathRole).toString().contains(process, Qt::CaseInsensitive);
			ui.processList->setRowHidden(i, !matches);
		}
	});
	connect(ui.processEdit, &QLineEdit::returnPressed, this, &QDialog::accept);
}

QString AttachProcessDialog::SelectedProcess()
{
	QString process = ui.processEdit->text();
	int visibleRows = 0, visibleRow = -1;
	for (int i = 0; i < model.rowCount(); ++i)
	{
		auto item = model.item(i);
		if (item->text() == process) return item->data(ProcessIdRole).toString();
		if (!ui.processList->isRowHidden(i))
		{
			++visibleRows;
			visibleRow = i;
		}
	}
	if (visibleRows == 1 && !process.isEmpty()) return model.item(visibleRow)->data(ProcessIdRole).toString();
	return process;
}
