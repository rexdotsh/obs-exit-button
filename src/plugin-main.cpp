#include <obs-frontend-api.h>
#include <obs-module.h>
#include <plugin-support.h>

#include <QDockWidget>
#include <QMainWindow>
#include <QMetaObject>
#include <QPointer>
#include <QPushButton>
#include <QSizePolicy>
#include <QThread>
#include <QVBoxLayout>

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

namespace {
QPointer<QPushButton> exit_button;
bool unloading = false;

void add_exit_button(QMainWindow *main_window)
{
	if (unloading || exit_button) {
		return;
	}

	auto *controls_dock = main_window->findChild<QDockWidget *>("controlsDock");
	auto *controls = controls_dock ? controls_dock->widget() : nullptr;
	auto *layout = controls ? controls->findChild<QVBoxLayout *>("buttonsVLayout") : nullptr;

	if (!controls || !layout) {
		plugin_log(LOG_WARNING, "could not find the OBS Controls dock layout");
		return;
	}

	if (controls->findChild<QPushButton *>("exitButton")) {
		plugin_log(LOG_INFO, "an Exit button already exists; nothing to do");
		return;
	}

	const char *translated_text = obs_frontend_get_locale_string("Exit");
	auto *button = new QPushButton(QString::fromUtf8(translated_text ? translated_text : "Exit"), controls);
	button->setObjectName("exitButton");
	button->setAccessibleName(button->text());
	button->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
	button->setMinimumWidth(150);

	int insertion_index = layout->count();
	if (insertion_index > 0 && layout->itemAt(insertion_index - 1)->spacerItem()) {
		--insertion_index;
	}

	layout->insertWidget(insertion_index, button);
	QObject::connect(button, &QPushButton::clicked, main_window, &QWidget::close);
	exit_button = button;

	plugin_log(LOG_INFO, "restored the Exit button");
}

void install_button()
{
	auto *main_window = static_cast<QMainWindow *>(obs_frontend_get_main_window());
	if (!main_window) {
		plugin_log(LOG_WARNING, "OBS main window is not available yet");
		return;
	}

	if (QThread::currentThread() == main_window->thread()) {
		add_exit_button(main_window);
	} else {
		QMetaObject::invokeMethod(main_window, [main_window]() { add_exit_button(main_window); },
				  Qt::BlockingQueuedConnection);
	}
}

void remove_button()
{
	if (!exit_button) {
		return;
	}

	auto *button = exit_button.data();
	if (QThread::currentThread() == button->thread()) {
		delete button;
	} else {
		QMetaObject::invokeMethod(button, [button]() { delete button; }, Qt::BlockingQueuedConnection);
	}

	exit_button.clear();
}

void frontend_event(enum obs_frontend_event event, void *)
{
	if (event == OBS_FRONTEND_EVENT_FINISHED_LOADING) {
		install_button();
	}
}
} // namespace

bool obs_module_load(void)
{
	unloading = false;
	obs_frontend_add_event_callback(frontend_event, nullptr);
	install_button();
	plugin_log(LOG_INFO, "loaded (version %s)", PLUGIN_VERSION);
	return true;
}

void obs_module_post_load(void)
{
	install_button();
}

void obs_module_unload(void)
{
	unloading = true;
	obs_frontend_remove_event_callback(frontend_event, nullptr);
	remove_button();
	plugin_log(LOG_INFO, "unloaded");
}

const char *obs_module_name(void)
{
	return "OBS Exit Button";
}

const char *obs_module_description(void)
{
	return "Restores the Exit button removed from the OBS Controls dock.";
}
