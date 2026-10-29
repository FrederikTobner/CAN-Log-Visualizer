/*****************************************************************************
 * Copyright (C) 2026 by Frederik Tobner                                     *
 *                                                                           *
 * This file is part of the CAN Log Visualizer.                              *
 *                                                                           *
 * Permission to use, copy, modify, and distribute this software and its     *
 * documentation under the terms of the GNU Affero General Public License is *
 * hereby granted.                                                           *
 * No representations are made about the suitability of this software for    *
 * any purpose.                                                              *
 * It is provided "as is" without express or implied warranty.               *
 * See the <https://www.gnu.org/licenses/gpl-3.0.html>                       *
 * GNU General Public License version 3 or later.                            *
 * License for more details.                                                 *
 ****************************************************************************/

#include "HelpDialog.hpp"

#include <QDialog>
#include <QTextBrowser>
#include <QVBoxLayout>

namespace {

constexpr auto kHelpText = R"(# Quick reference

## Capturing
Enter an interface name (default `vcan0`) and press **Connect**. Pass a candump log
on the command line or use **File > Open Log...** to inspect a recording.
**File > Save Log As...** writes the current trace in the same format.

## Filtering
Type a filter next to the Connect button and press Enter. Only matching frames are
shown, all frames are kept, and clearing the field shows everything again.

| Filter     | Shows                                              |
| ---------- | -------------------------------------------------- |
| `123`      | Standard frames with ID 123                        |
| `700:700`  | Standard frames where `ID & 700 == 700` (700..7FF) |
| `12345678` | The extended frame 12345678                        |
| `123, 456` | Frames matching any listed ID                      |
| `!7FF`     | Everything except 7FF (an exclude always wins)     |

## Statistics
The Statistics dock (View menu) lists every ID seen, with count, rate and cycle time.
Click a column header to sort. Statistics ignore the trace filter.

* **Rate** is measured over the last second, so IDs sent slower than 1 Hz may show 0.
* **Cycle** is the time between consecutive frames of one ID.
* **Bus load** is an upper bound assuming worst-case bit stuffing at the selected
  bitrate. Set the bitrate to match your bus.
* **Clear** resets the trace and the statistics.
)";

} // namespace

void showHelpDialog(QWidget * parent) {
    auto * dialog = new QDialog(parent);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle("Quick reference");
    dialog->resize(620, 560);

    auto * browser = new QTextBrowser(dialog);
    browser->setMarkdown(kHelpText);

    auto * layout = new QVBoxLayout(dialog);
    layout->addWidget(browser);
    dialog->open();
}
