; First is default
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Russian.nlf"

; Language selection dialog
LangString InstallerLanguageTitle  ${LANG_RUSSIAN} "Язык установки"
LangString SelectInstallerLanguage  ${LANG_RUSSIAN} "Выберите язык программы установки"

; subtitle on license text caption
LangString LicenseSubTitleUpdate ${LANG_RUSSIAN} "Обновление"
LangString LicenseSubTitleSetup ${LANG_RUSSIAN} "Настройка"

LangString MULTIUSER_TEXT_INSTALLMODE_TITLE ${LANG_RUSSIAN} "Режим Установки"
LangString MULTIUSER_TEXT_INSTALLMODE_SUBTITLE ${LANG_RUSSIAN} "Установить для всех пользователей (требуется разрешение администратора) или только для текущего пользователя?"
LangString MULTIUSER_INNERTEXT_INSTALLMODE_TOP ${LANG_RUSSIAN} "Когда вы запускаете этот инсталлятор с правами Администратора, вы можете выбрать директорию установки в корневой каталог c:\Program Files или (например) в папку текущего пользователя AppData\Local."
LangString MULTIUSER_INNERTEXT_INSTALLMODE_ALLUSERS ${LANG_RUSSIAN} "Установить для всех пользователей"
LangString MULTIUSER_INNERTEXT_INSTALLMODE_CURRENTUSER ${LANG_RUSSIAN} "Установить только для текущего пользователя"

; installation directory text
LangString DirectoryChooseTitle ${LANG_RUSSIAN} "Каталог установки"
LangString DirectoryChooseUpdate ${LANG_RUSSIAN} "Выберите каталог Firestorm для обновления до версии ${VERSION_LONG}.(XXX):"
LangString DirectoryChooseSetup ${LANG_RUSSIAN} "Выберите каталог для установки Firestorm:"

LangString MUI_TEXT_DIRECTORY_TITLE ${LANG_RUSSIAN} "Каталог Установки"
LangString MUI_TEXT_DIRECTORY_SUBTITLE ${LANG_RUSSIAN} "Выберите каталог для установки Firestorm:"

LangString MUI_TEXT_INSTALLING_TITLE ${LANG_RUSSIAN} "Установка Firestorm…"
LangString MUI_TEXT_INSTALLING_SUBTITLE ${LANG_RUSSIAN} "Установка приложения Firestorm Viewer в $INSTDIR"

LangString MUI_TEXT_FINISH_TITLE ${LANG_RUSSIAN} "Установка Firestorm"
LangString MUI_TEXT_FINISH_SUBTITLE ${LANG_RUSSIAN} "Приложение Firestorm Viewer установлено в $INSTDIR."

LangString MUI_TEXT_ABORT_TITLE ${LANG_RUSSIAN} "Установка Прервана"
LangString MUI_TEXT_ABORT_SUBTITLE ${LANG_RUSSIAN} "Не устанавливать приложение Firestorm Viewer в $INSTDIR."

; CheckStartupParams message box
LangString CheckStartupParamsMB ${LANG_RUSSIAN} "Не удалось найти программу «$INSTNAME». Автоматическое обновление не выполнено."

; installation success dialog
LangString InstSuccesssQuestion ${LANG_RUSSIAN} "Запустить Firestorm?"

; remove old NSIS version
LangString RemoveOldNSISVersion ${LANG_RUSSIAN} "Поиск прежней версии..."

; check windows version
LangString CheckWindowsVersionDP ${LANG_RUSSIAN} "Проверка версии Windows..."
LangString CheckWindowsVersionMB ${LANG_RUSSIAN} "Firestorm может работать только в Windows Vista SP2 или новее.$\nУстановка в этой операционной системе не поддерживается. Выход."
LangString CheckWindowsServPackMB ${LANG_RUSSIAN} "Рекомендуется запускать Firestorm с последним пакетом обновления для вашей операционной системы.$\nЭто поможет повысить производительность и стабильность программы."
LangString UseLatestServPackDP ${LANG_RUSSIAN} "Пожалуйста, используйте Центр обновления Windows, чтобы установить последний пакет обновления."

; checkifadministrator function (install)
LangString CheckAdministratorInstDP ${LANG_RUSSIAN} "Проверка разрешений на установку..."
LangString CheckAdministratorInstMB ${LANG_RUSSIAN} 'Вероятно, у вас ограниченный аккаунт.$\nДля установки Firestorm необходимы права администратора.'

; checkifadministrator function (uninstall)
LangString CheckAdministratorUnInstDP ${LANG_RUSSIAN} "Проверка разрешений на удаление..."
LangString CheckAdministratorUnInstMB ${LANG_RUSSIAN} 'Вероятно, у вас ограниченный аккаунт.$\nДля удаления Firestorm необходимы права администратора.'

; checkcpuflags
LangString MissingSSE2 ${LANG_RUSSIAN} "Возможно, на этом компьютере нет ЦП с поддержкой SSE2, которая необходима для работы Firestorm ${VERSION_LONG}. Продолжить?"

; Extended cpu checks (AVX2)
LangString MissingAVX2 ${LANG_RUSSIAN} "Ваш процессор не поддерживает инструкции AVX2. Пожалуйста, скачайте версию для устаревших процессоров по ссылке: %DLURL%-legacy-cpus/"
LangString AVX2Available ${LANG_RUSSIAN} "Ваш процессор поддерживает инструкции AVX2. Вы можете скачать оптимизированную версию AVX2 для повышения производительности по ссылке: %DLURL%/. Хотите скачать её сейчас?"
LangString AVX2OverrideConfirmation ${LANG_RUSSIAN} "If you believe that your PC can support AVX2 optimization, you may install anyway. Do you want to proceed?"
LangString AVX2OverrideNote ${LANG_RUSSIAN} "By overriding the installer, you are about to install a version that might crash immediately after launching. If this happens, please immediately install the standard CPU version."

; closesecondlife function (install)
LangString CloseSecondLifeInstDP ${LANG_RUSSIAN} "Ожидаю завершения работы Firestorm..."
LangString CloseSecondLifeInstMB ${LANG_RUSSIAN} "Firestorm все еще работает, выполнить установку невозможно.$\n$\nЗавершите текущую операцию и нажмите кнопку «OK», чтобы закрыть Firestorm и продолжить установку.$\nНажмите кнопку «ОТМЕНА» для отказа от установки."
LangString CloseSecondLifeInstRM ${LANG_RUSSIAN} "Firestorm failed to remove some files from a previous install."

; closesecondlife function (uninstall)
LangString CloseSecondLifeUnInstDP ${LANG_RUSSIAN} "Ожидаю завершения работы Firestorm..."
LangString CloseSecondLifeUnInstMB ${LANG_RUSSIAN} "Firestorm все еще работает, выполнить удаление невозможно.$\n$\nЗавершите текущую операцию и нажмите кнопку «OK», чтобы закрыть Firestorm и продолжить удаление.$\nНажмите кнопку «ОТМЕНА» для отказа от удаления."

; CheckNetworkConnection
LangString CheckNetworkConnectionDP ${LANG_RUSSIAN} "Проверка подключения к сети..."

; error during installation
LangString ErrorSecondLifeInstallRetry ${LANG_RUSSIAN} "Firestorm installer encountered issues during installation. Some files may not have been copied correctly."
LangString ErrorSecondLifeInstallSupport ${LANG_RUSSIAN} "Please reinstall viewer from https://www.firestormviewer.org/downloads/ and contact https://www.firestormviewer.org/support/ if issue persists after reinstall."

; ask to remove user's data files
LangString RemoveDataFilesMB ${LANG_RUSSIAN} "Удаление файлов кэша из папки «Documents and Settings»?"

; delete program files
LangString DeleteProgramFilesMB ${LANG_RUSSIAN} "В каталоге программы Firestorm остались файлы.$\n$\nВероятно, это файлы, созданные или перемещенные вами в $\n$INSTDIR$\n$\nУдалить их?"

; uninstall text
LangString UninstallTextMsg ${LANG_RUSSIAN} "Программа Firestorm ${VERSION_LONG} будет удалена из вашей системы."

; ask to remove registry keys that still might be needed by other viewers that are installed
LangString DeleteRegistryKeysMB ${LANG_RUSSIAN} "Вы хотите удалить ключи реестра приложений?$\n$\nРекомендуется сохранить ключи реестра, если у вас установлены другие версии Firestorm."

; <FS:Ansariel> Ask to create protocol handler registry entries
LangString CreateUrlRegistryEntries ${LANG_RUSSIAN} "Вы хотите зарегистрировать Firestorm в качестве обработчика по умолчанию для протоколов виртуального мира?$\n$\nЕсли у вас установлены другие версии Firestorm, существующие ключи реестра будут перезаписаны."

; <FS:Ansariel> Optional start menu entry
LangString CreateStartMenuEntry ${LANG_RUSSIAN} "Создать запись в стартовом меню?"

; <FS:Ansariel> Application name suffix for OpenSim variant
LangString ForOpenSimSuffix ${LANG_RUSSIAN} "для OpenSimulator"

LangString DeleteDocumentAndSettingsDP ${LANG_RUSSIAN} "Удаление файлов в папке «Документы и настройки»."
LangString UnChatlogsNoticeMB ${LANG_RUSSIAN} "Эта деинсталляция НЕ удалит ваши журналы чата Firestorm и другие личные файлы. Если вы хотите сделать это самостоятельно, удалите папку Firestorm в папке данных пользовательского приложения."
LangString UnRemovePasswordsDP ${LANG_RUSSIAN} "Удаление сохраненных паролей Firestorm."

LangString MUI_TEXT_LICENSE_TITLE ${LANG_RUSSIAN} "Лицензионное соглашение на систему голосовой связи Vivox"
LangString MUI_TEXT_LICENSE_SUBTITLE ${LANG_RUSSIAN} "Дополнительное лицензионное соглашение для компонентов голосовой системы Vivox."
LangString MUI_INNERTEXT_LICENSE_TOP ${LANG_RUSSIAN} "Пожалуйста, внимательно прочтите следующее лицензионное соглашение, прежде чем продолжить установку:"
LangString MUI_INNERTEXT_LICENSE_BOTTOM ${LANG_RUSSIAN} "Вы должны согласиться с условиями лицензии, чтобы продолжить установку."
