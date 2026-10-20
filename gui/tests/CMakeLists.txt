# Widgets need a platform plugin; offscreen works without a display (CI).
add_qt_tests(LIBRARIES CanLogGui ENVIRONMENT QT_QPA_PLATFORM=offscreen)
