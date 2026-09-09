#include "TaskModal.hpp"
#include "UIFrame.hpp"
#include "../core/Localization.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <iomanip>

namespace Render {

TaskModal::TaskModal()
    : m_isOpen(false),
      m_btnClose(Rectangle{0, 0, 0, 0}, "X", "", Color{180, 40, 40, 255}, Color{255, 80, 80, 255}),
      m_btnTabAll(Rectangle{0, 0, 0, 0}, "TUM GOREVLER", "", Color{28, 38, 55, 255}, Color{60, 180, 255, 255}),
      m_btnTabCash(Rectangle{0, 0, 0, 0}, "GELIR ($)", "", Color{25, 42, 35, 255}, Color{50, 220, 130, 255}),
      m_btnTabCrypto(Rectangle{0, 0, 0, 0}, "KRIPTO (COIN)", "", Color{35, 30, 52, 255}, Color{170, 110, 255, 255}),
      m_btnTabFacility(Rectangle{0, 0, 0, 0}, "FABRIKA", "", Color{45, 35, 22, 255}, Color{255, 170, 40, 255}),
      m_btnTabHardware(Rectangle{0, 0, 0, 0}, "DONANIM", "", Color{22, 40, 50, 255}, Color{40, 210, 230, 255}),
      m_btnTabTrading(Rectangle{0, 0, 0, 0}, "TICARET", "", Color{35, 25, 35, 255}, Color{255, 100, 160, 255})
{
    for (int i = 0; i < 90; ++i) {
        m_claimButtons.emplace_back(Rectangle{0, 0, 0, 0}, "ODUL AL", "", Color{180, 140, 20, 255}, Color{255, 210, 40, 255});
    }
}

void TaskModal::Open() {
    m_isOpen = true;
    m_scrollOffset = 0.0f;
}

void TaskModal::Close() {
    m_isOpen = false;
}

bool TaskModal::IsOpen() const {
    return m_isOpen;
}

void TaskModal::Update(Core::TaskManager& taskManager, Core::EconomyManager& economy) {
    if (!m_isOpen) return;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    float modalW = std::clamp(static_cast<float>(screenW) * 0.90f, 920.0f, 1100.0f);
    float modalH = std::clamp(static_cast<float>(screenH) * 0.90f, 620.0f, 800.0f);
    float modalX = (screenW - modalW) * 0.5f;
    float modalY = (screenH - modalH) * 0.5f;

    // Close button
    m_btnClose.SetBounds(Rectangle{modalX + modalW - 46.0f, modalY + 12.0f, 34.0f, 34.0f});
    if (m_btnClose.UpdateAndCheckClick() || IsKeyPressed(KEY_ESCAPE)) {
        Close();
        return;
    }

    // Category Tabs (6 Tabs)
    const float tabY = modalY + 54.0f;
    const float tabH = 34.0f;
    const float tabGap = 6.0f;
    const float tabW = (modalW - 40.0f - (5.0f * tabGap)) / 6.0f;

    m_btnTabAll.SetBounds(Rectangle{modalX + 20.0f + 0 * (tabW + tabGap), tabY, tabW, tabH});
    m_btnTabCash.SetBounds(Rectangle{modalX + 20.0f + 1 * (tabW + tabGap), tabY, tabW, tabH});
    m_btnTabCrypto.SetBounds(Rectangle{modalX + 20.0f + 2 * (tabW + tabGap), tabY, tabW, tabH});
    m_btnTabFacility.SetBounds(Rectangle{modalX + 20.0f + 3 * (tabW + tabGap), tabY, tabW, tabH});
    m_btnTabHardware.SetBounds(Rectangle{modalX + 20.0f + 4 * (tabW + tabGap), tabY, tabW, tabH});
    m_btnTabTrading.SetBounds(Rectangle{modalX + 20.0f + 5 * (tabW + tabGap), tabY, tabW, tabH});

    if (m_btnTabAll.UpdateAndCheckClick()) { m_currentCategory = Core::TaskCategory::ALL; m_scrollOffset = 0.0f; }
    if (m_btnTabCash.UpdateAndCheckClick()) { m_currentCategory = Core::TaskCategory::CASH; m_scrollOffset = 0.0f; }
    if (m_btnTabCrypto.UpdateAndCheckClick()) { m_currentCategory = Core::TaskCategory::CRYPTO; m_scrollOffset = 0.0f; }
    if (m_btnTabFacility.UpdateAndCheckClick()) { m_currentCategory = Core::TaskCategory::FACILITY; m_scrollOffset = 0.0f; }
    if (m_btnTabHardware.UpdateAndCheckClick()) { m_currentCategory = Core::TaskCategory::HARDWARE; m_scrollOffset = 0.0f; }
    if (m_btnTabTrading.UpdateAndCheckClick()) { m_currentCategory = Core::TaskCategory::TRADING; m_scrollOffset = 0.0f; }

    const auto& tasks = taskManager.GetTasks();
    auto visibleIndices = taskManager.GetTaskIndicesByCategory(m_currentCategory);

    // Mouse wheel scroll handling
    Vector2 mousePos = GetMousePosition();
    const float contentY = tabY + tabH + 12.0f;
    const float contentH = modalH - (contentY - modalY) - 16.0f;
    Rectangle contentArea{modalX + 20.0f, contentY, modalW - 40.0f, contentH};

    if (CheckCollisionPointRec(mousePos, contentArea)) {
        float wheel = GetMouseWheelMove();
        m_scrollOffset += wheel * 42.0f;
    }

    const float itemH = 82.0f;
    const float gap = 8.0f;
    float totalContentH = visibleIndices.size() * (itemH + gap);
    float maxScroll = std::max(0.0f, totalContentH - contentArea.height);
    m_scrollOffset = std::clamp(m_scrollOffset, -maxScroll, 0.0f);

    if (m_claimButtons.size() < tasks.size()) {
        m_claimButtons.resize(tasks.size(), UIButton(Rectangle{0, 0, 0, 0}, "ODUL AL", "", Color{180, 140, 20, 255}, Color{255, 210, 40, 255}));
    }

    for (size_t vIdx = 0; vIdx < visibleIndices.size(); ++vIdx) {
        size_t taskIdx = visibleIndices[vIdx];
        const auto& task = tasks[taskIdx];
        float rowY = contentArea.y + m_scrollOffset + vIdx * (itemH + gap);

        Rectangle btnRec{contentArea.x + contentArea.width - 150.0f, rowY + 23.0f, 138.0f, 36.0f};
        m_claimButtons[taskIdx].SetBounds(btnRec);

        if (rowY + itemH >= contentArea.y && rowY <= contentArea.y + contentArea.height) {
            if (m_claimButtons[taskIdx].UpdateAndCheckClick()) {
                if (task.isCompleted && !task.isClaimed && task.isUnlocked) {
                    taskManager.ClaimReward(task.id, economy);
                }
            }
        }
    }
}

void TaskModal::Draw(const Core::TaskManager& taskManager, const Core::EconomyManager& economy) {
    if (!m_isOpen) return;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    DrawRectangle(0, 0, screenW, screenH, Color{8, 12, 18, 225});

    float modalW = std::clamp(static_cast<float>(screenW) * 0.90f, 920.0f, 1100.0f);
    float modalH = std::clamp(static_cast<float>(screenH) * 0.90f, 620.0f, 800.0f);
    float modalX = (screenW - modalW) * 0.5f;
    float modalY = (screenH - modalH) * 0.5f;

    // Modal Outer Frame
    Rectangle modalRec{modalX, modalY, modalW, modalH};
    UIFrame::DrawCard(modalRec, "", Color{65, 145, 245, 255});

    // Header Banner
    DrawRectangle(static_cast<int>(modalX), static_cast<int>(modalY), static_cast<int>(modalW), 46, Color{18, 26, 42, 255});
    DrawLine(static_cast<int>(modalX), static_cast<int>(modalY + 46), static_cast<int>(modalX + modalW), static_cast<int>(modalY + 46), Color{65, 145, 245, 180});

    std::string titleText = std::string("GOREVLER & ODULLER - ") + Core::LocalizationManager::Tr("TASK_MODAL_TITLE");
    UIFrame::DrawTextCustom(titleText, modalX + 24.0f, modalY + 12.0f, 22.0f, Color{255, 220, 80, 255}, true);

    size_t unclaimedCount = taskManager.GetUnclaimedCompletedCount();
    if (unclaimedCount > 0) {
        std::string badgeBuf = std::to_string(unclaimedCount) + " " + Core::LocalizationManager::Tr("TASK_READY_BADGE");
        float badgeW = UIFrame::MeasureTextCustom(badgeBuf, 14.0f, true) + 20.0f;
        Rectangle badgeRec{modalX + modalW - 230.0f, modalY + 10.0f, badgeW, 28.0f};
        DrawRectangleRounded(badgeRec, 0.4f, 4, Color{240, 160, 20, 255});
        UIFrame::DrawTextCustom(badgeBuf, badgeRec.x + 10.0f, badgeRec.y + 6.0f, 14.0f, Color{15, 15, 15, 255}, true);
    }

    m_btnClose.Draw();

    // Category Tabs
    m_btnTabAll.Draw();
    m_btnTabCash.Draw();
    m_btnTabCrypto.Draw();
    m_btnTabFacility.Draw();
    m_btnTabHardware.Draw();
    m_btnTabTrading.Draw();

    // Active Tab Underline Neon
    UIButton* activeTab = nullptr;
    Color accentColor = Color{60, 180, 255, 255};
    if (m_currentCategory == Core::TaskCategory::ALL) { activeTab = &m_btnTabAll; accentColor = Color{60, 180, 255, 255}; }
    else if (m_currentCategory == Core::TaskCategory::CASH) { activeTab = &m_btnTabCash; accentColor = Color{50, 220, 130, 255}; }
    else if (m_currentCategory == Core::TaskCategory::CRYPTO) { activeTab = &m_btnTabCrypto; accentColor = Color{170, 110, 255, 255}; }
    else if (m_currentCategory == Core::TaskCategory::FACILITY) { activeTab = &m_btnTabFacility; accentColor = Color{255, 170, 40, 255}; }
    else if (m_currentCategory == Core::TaskCategory::HARDWARE) { activeTab = &m_btnTabHardware; accentColor = Color{40, 210, 230, 255}; }
    else if (m_currentCategory == Core::TaskCategory::TRADING) { activeTab = &m_btnTabTrading; accentColor = Color{255, 100, 160, 255}; }

    if (activeTab) {
        Rectangle b = activeTab->GetBounds();
        DrawRectangle(static_cast<int>(b.x), static_cast<int>(b.y + b.height - 3), static_cast<int>(b.width), 3, accentColor);
    }

    // Content Area with Scissor
    const float tabY = modalY + 54.0f;
    const float tabH = 34.0f;
    const float contentY = tabY + tabH + 12.0f;
    const float contentH = modalH - (contentY - modalY) - 16.0f;
    Rectangle contentArea{modalX + 20.0f, contentY, modalW - 40.0f, contentH};

    BeginScissorMode(static_cast<int>(contentArea.x), static_cast<int>(contentArea.y),
                     static_cast<int>(contentArea.width), static_cast<int>(contentArea.height));

    const auto& tasks = taskManager.GetTasks();
    auto visibleIndices = taskManager.GetTaskIndicesByCategory(m_currentCategory);
    const float itemH = 82.0f;
    const float gap = 8.0f;

    for (size_t vIdx = 0; vIdx < visibleIndices.size(); ++vIdx) {
        size_t taskIdx = visibleIndices[vIdx];
        const auto& task = tasks[taskIdx];
        float rowY = contentArea.y + m_scrollOffset + vIdx * (itemH + gap);

        // Off-screen culling
        if (rowY + itemH < contentArea.y || rowY > contentArea.y + contentArea.height) {
            continue;
        }

        Rectangle rowRec{contentArea.x, rowY, contentArea.width, itemH};

        // Card colors
        Color cardBg = Color{20, 28, 44, 245};
        Color cardBorder = Color{45, 65, 95, 230};

        if (!task.isUnlocked) {
            cardBg = Color{16, 20, 28, 200};
            cardBorder = Color{60, 50, 45, 150};
        } else if (task.isClaimed) {
            cardBg = Color{16, 28, 24, 220};
            cardBorder = Color{40, 130, 85, 180};
        } else if (task.isCompleted) {
            cardBg = Color{35, 38, 22, 255};
            cardBorder = Color{240, 190, 40, 240};
        }

        DrawRectangleRounded(rowRec, 0.08f, 6, cardBg);
        DrawRectangleRoundedLines(rowRec, 0.08f, 6, 1.5f, cardBorder);

        // Title & Category tag
        const char* title = Core::LocalizationManager::Tr(task.titleKey);
        const char* desc = Core::LocalizationManager::Tr(task.descKey);

        Color titleCol = task.isCompleted ? Color{255, 225, 100, 255} : (task.isUnlocked ? Color{240, 246, 255, 255} : Color{140, 150, 165, 255});
        UIFrame::DrawTextCustom(title, rowRec.x + 16.0f, rowRec.y + 11.0f, 16.5f, titleCol, true);

        if (!task.isUnlocked) {
            std::string lockMsg = "[KILITLI - " + task.requiredFacilityId + " Satin Alindiginda Acilir]";
            if (task.requiredFacilityId == "FAC_ICELAND") lockMsg = "[KILITLI - Izlanda Tesisi Satin Alindiginda Acilir]";
            else if (task.requiredFacilityId == "FAC_NORWAY") lockMsg = "[KILITLI - Norvec Tesisi Satin Alindiginda Acilir]";
            else if (task.requiredFacilityId == "FAC_GERMANY") lockMsg = "[KILITLI - Almanya Tesisi Satin Alindiginda Acilir]";
            else if (task.requiredFacilityId == "FAC_SIBERIA") lockMsg = "[KILITLI - Sibirya Tesisi Satin Alindiginda Acilir]";
            UIFrame::DrawTextCustom(lockMsg, rowRec.x + 16.0f, rowRec.y + 33.0f, 12.0f, Color{255, 130, 80, 255}, true);
        } else {
            UIFrame::DrawTextCustom(desc, rowRec.x + 16.0f, rowRec.y + 33.0f, 12.5f, Color{165, 185, 210, 255}, false);
        }

        // Dual Reward Badge (Cash + Crypto)
        std::string rewardStr = "ODUL: ";
        if (task.rewardCash > 0.0 && !task.rewardCryptoSymbol.empty()) {
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(task.rewardCryptoAmount < 1.0 ? 2 : 1) << task.rewardCryptoAmount;
            rewardStr += economy.FormatFiat(task.rewardCash) + " + " + ss.str() + " " + task.rewardCryptoSymbol;
        } else if (task.rewardCash > 0.0) {
            rewardStr += economy.FormatFiat(task.rewardCash);
        } else if (!task.rewardCryptoSymbol.empty()) {
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(task.rewardCryptoAmount < 1.0 ? 2 : 1) << task.rewardCryptoAmount;
            rewardStr += ss.str() + " " + task.rewardCryptoSymbol;
        }

        Color rewCol = (!task.rewardCryptoSymbol.empty()) ? Color{0, 235, 180, 255} : Color{70, 235, 140, 255};
        UIFrame::DrawTextCustom(rewardStr, rowRec.x + 16.0f, rowRec.y + 55.0f, 14.0f, rewCol, true);

        // Progress Bar
        const float barW = rowRec.width - 490.0f;
        const float barX = rowRec.x + 310.0f;
        const float barY = rowRec.y + 48.0f;
        const float barH = 15.0f;

        if (barW > 80.0f) {
            DrawRectangleRounded(Rectangle{barX, barY, barW, barH}, 0.5f, 4, Color{14, 20, 30, 255});
            float progressRatio = std::clamp(static_cast<float>(task.currentProgress / task.targetProgress), 0.0f, 1.0f);
            if (progressRatio > 0.001f) {
                Color progColor = task.isCompleted ? Color{50, 220, 120, 255} : Color{50, 175, 245, 255};
                DrawRectangleRounded(Rectangle{barX, barY, barW * progressRatio, barH}, 0.5f, 4, progColor);
            }
            DrawRectangleRoundedLines(Rectangle{barX, barY, barW, barH}, 0.5f, 4, 1.0f, Color{60, 80, 110, 200});

            // Progress text
            char progressBuf[64];
            if (task.targetProgress >= 1000.0) {
                snprintf(progressBuf, sizeof(progressBuf), "%.0f / %.0f", task.currentProgress, task.targetProgress);
            } else if (task.targetProgress <= 5.0) {
                snprintf(progressBuf, sizeof(progressBuf), "%.1f / %.1f", task.currentProgress, task.targetProgress);
            } else {
                snprintf(progressBuf, sizeof(progressBuf), "%.0f / %.0f", task.currentProgress, task.targetProgress);
            }
            float tw = UIFrame::MeasureTextCustom(progressBuf, 11.0f, true);
            UIFrame::DrawTextCustom(progressBuf, barX + (barW - tw) * 0.5f, barY + 1.5f, 11.0f, Color{245, 245, 245, 255}, true);
        }

        // Action Button
        Rectangle btnRec = m_claimButtons[taskIdx].GetBounds();
        if (!task.isUnlocked) {
            DrawRectangleRounded(btnRec, 0.2f, 4, Color{24, 28, 36, 180});
            DrawRectangleRoundedLines(btnRec, 0.2f, 4, 1.0f, Color{50, 55, 70, 200});
            const char* lockText = "KILITLI";
            float tw = UIFrame::MeasureTextCustom(lockText, 13.0f, true);
            UIFrame::DrawTextCustom(lockText, btnRec.x + (btnRec.width - tw) * 0.5f, btnRec.y + 10.0f, 13.0f, Color{120, 130, 145, 255}, true);
        } else if (task.isClaimed) {
            DrawRectangleRounded(btnRec, 0.2f, 4, Color{22, 45, 32, 200});
            DrawRectangleRoundedLines(btnRec, 0.2f, 4, 1.0f, Color{40, 140, 80, 200});
            const char* claimedText = "ALINDI \xc2\xa0\xe2\x9c\x93"; // ALINDI ✓
            float tw = UIFrame::MeasureTextCustom("ALINDI", 13.5f, true);
            UIFrame::DrawTextCustom("ALINDI ✓", btnRec.x + (btnRec.width - tw) * 0.5f - 8.0f, btnRec.y + 9.0f, 13.5f, Color{70, 220, 130, 255}, true);
        } else if (task.isCompleted) {
            float pulse = 0.82f + 0.18f * std::sin(static_cast<float>(GetTime() * 6.0));
            Color claimColor = Color{static_cast<unsigned char>(240 * pulse), static_cast<unsigned char>(180 * pulse), 30, 255};
            DrawRectangleRounded(btnRec, 0.2f, 4, claimColor);
            DrawRectangleRoundedLines(btnRec, 0.2f, 4, 1.5f, Color{255, 240, 120, 255});
            const char* claimText = "ODULU AL!";
            float tw = UIFrame::MeasureTextCustom(claimText, 14.0f, true);
            UIFrame::DrawTextCustom(claimText, btnRec.x + (btnRec.width - tw) * 0.5f, btnRec.y + 9.0f, 14.0f, Color{15, 15, 15, 255}, true);
        } else {
            DrawRectangleRounded(btnRec, 0.2f, 4, Color{28, 38, 52, 180});
            DrawRectangleRoundedLines(btnRec, 0.2f, 4, 1.0f, Color{50, 70, 95, 200});
            const char* progText = "DEVAM EDIYOR";
            float tw = UIFrame::MeasureTextCustom(progText, 12.5f, true);
            UIFrame::DrawTextCustom(progText, btnRec.x + (btnRec.width - tw) * 0.5f, btnRec.y + 10.0f, 12.5f, Color{140, 160, 185, 255}, true);
        }
    }

    EndScissorMode();
}

} // namespace Render

