#include "TaskModal.hpp"
#include "UIFrame.hpp"
#include "../core/Localization.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace Render {

TaskModal::TaskModal()
    : m_isOpen(false),
      m_btnClose(Rectangle{0, 0, 0, 0}, "X", "", Color{180, 40, 40, 255}, Color{255, 80, 80, 255})
{
    // Pre-allocate claim buttons
    for (int i = 0; i < 20; ++i) {
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

    float modalW = std::min(860.0f, screenW - 40.0f);
    float modalH = std::min(620.0f, screenH - 60.0f);
    float modalX = (screenW - modalW) * 0.5f;
    float modalY = (screenH - modalH) * 0.5f;

    // Close button update
    m_btnClose.SetBounds(Rectangle{modalX + modalW - 45.0f, modalY + 12.0f, 32.0f, 32.0f});
    if (m_btnClose.UpdateAndCheckClick()) {
        Close();
        return;
    }

    const auto& tasks = taskManager.GetTasks();

    // Mouse wheel scroll handling
    Vector2 mousePos = GetMousePosition();
    Rectangle contentArea{modalX + 20.0f, modalY + 70.0f, modalW - 40.0f, modalH - 90.0f};
    
    if (CheckCollisionPointRec(mousePos, contentArea)) {
        float wheel = GetMouseWheelMove();
        m_scrollOffset += wheel * 36.0f;
    }

    float itemH = 80.0f;
    float gap = 10.0f;
    float totalContentH = tasks.size() * (itemH + gap);
    float maxScroll = std::max(0.0f, totalContentH - contentArea.height);
    m_scrollOffset = std::clamp(m_scrollOffset, -maxScroll, 0.0f);

    // Update claim buttons
    if (m_claimButtons.size() < tasks.size()) {
        m_claimButtons.resize(tasks.size(), UIButton(Rectangle{0, 0, 0, 0}, "ODUL AL", "", Color{180, 140, 20, 255}, Color{255, 210, 40, 255}));
    }

    for (size_t i = 0; i < tasks.size(); ++i) {
        const auto& task = tasks[i];
        float rowY = contentArea.y + m_scrollOffset + i * (itemH + gap);
        
        Rectangle btnRec{contentArea.x + contentArea.width - 150.0f, rowY + 22.0f, 135.0f, 36.0f};
        m_claimButtons[i].SetBounds(btnRec);

        if (rowY + itemH >= contentArea.y && rowY <= contentArea.y + contentArea.height) {
            if (m_claimButtons[i].UpdateAndCheckClick()) {
                if (task.isCompleted && !task.isClaimed) {
                    taskManager.ClaimReward(task.id, economy);
                }
            }
        }
    }
}

void TaskModal::Draw(const Core::TaskManager& taskManager, const Core::EconomyManager& economy) const {
    if (!m_isOpen) return;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // Dark semi-transparent backdrop
    DrawRectangle(0, 0, screenW, screenH, Color{10, 12, 18, 215});

    float modalW = std::min(860.0f, screenW - 40.0f);
    float modalH = std::min(620.0f, screenH - 60.0f);
    float modalX = (screenW - modalW) * 0.5f;
    float modalY = (screenH - modalH) * 0.5f;

    // Modal Outer Frame
    Rectangle modalRec{modalX, modalY, modalW, modalH};
    DrawRectangleRounded(modalRec, 0.03f, 8, Color{18, 24, 38, 252});
    DrawRectangleRoundedLines(modalRec, 0.03f, 8, 2.0f, Color{65, 140, 245, 220});

    // Top Header Banner
    DrawRectangleRounded(Rectangle{modalX + 10.0f, modalY + 10.0f, modalW - 20.0f, 48.0f}, 0.08f, 6, Color{28, 38, 58, 255});
    DrawRectangleRoundedLines(Rectangle{modalX + 10.0f, modalY + 10.0f, modalW - 20.0f, 48.0f}, 0.08f, 6, 1.0f, Color{90, 160, 255, 120});

    const char* titleText = Core::LocalizationManager::Tr("TASK_MODAL_TITLE");
    UIFrame::DrawTextCustom(titleText, modalX + 25.0f, modalY + 20.0f, 22.0f, Color{255, 220, 80, 255}, true);

    size_t unclaimedCount = taskManager.GetUnclaimedCompletedCount();
    if (unclaimedCount > 0) {
        char badgeBuf[48];
        snprintf(badgeBuf, sizeof(badgeBuf), "%zu %s", unclaimedCount, Core::LocalizationManager::Tr("TASK_READY_BADGE"));
        float badgeW = UIFrame::MeasureTextCustom(badgeBuf, 14.0f, true) + 20.0f;
        Rectangle badgeRec{modalX + modalW - 210.0f, modalY + 20.0f, badgeW, 28.0f};
        DrawRectangleRounded(badgeRec, 0.4f, 4, Color{240, 160, 20, 255});
        UIFrame::DrawTextCustom(badgeBuf, badgeRec.x + 10.0f, badgeRec.y + 6.0f, 14.0f, Color{15, 15, 15, 255}, true);
    }

    // Close Button
    m_btnClose.Draw();

    // Content Area with Scissor
    Rectangle contentArea{modalX + 20.0f, modalY + 70.0f, modalW - 40.0f, modalH - 90.0f};
    BeginScissorMode((int)contentArea.x, (int)contentArea.y, (int)contentArea.width, (int)contentArea.height);

    const auto& tasks = taskManager.GetTasks();
    float itemH = 80.0f;
    float gap = 10.0f;

    for (size_t i = 0; i < tasks.size(); ++i) {
        const auto& task = tasks[i];
        float rowY = contentArea.y + m_scrollOffset + i * (itemH + gap);

        // Cull off-screen items
        if (rowY + itemH < contentArea.y || rowY > contentArea.y + contentArea.height) {
            continue;
        }

        Rectangle rowRec{contentArea.x, rowY, contentArea.width, itemH};

        // Background Color based on status
        Color cardBg = Color{24, 32, 50, 255};
        Color cardBorder = Color{45, 65, 95, 255};

        if (task.isClaimed) {
            cardBg = Color{18, 28, 25, 220};
            cardBorder = Color{40, 120, 80, 180};
        } else if (task.isCompleted) {
            cardBg = Color{35, 38, 25, 255};
            cardBorder = Color{230, 180, 40, 220};
        }

        DrawRectangleRounded(rowRec, 0.08f, 6, cardBg);
        DrawRectangleRoundedLines(rowRec, 0.08f, 6, 1.5f, cardBorder);

        // Task Title & Description
        const char* title = Core::LocalizationManager::Tr(task.titleKey);
        const char* desc = Core::LocalizationManager::Tr(task.descKey);

        UIFrame::DrawTextCustom(title, rowRec.x + 16.0f, rowRec.y + 12.0f, 17.0f, 
                                task.isCompleted ? Color{255, 225, 110, 255} : Color{235, 240, 255, 255}, true);
        
        UIFrame::DrawTextCustom(desc, rowRec.x + 16.0f, rowRec.y + 34.0f, 13.0f, Color{150, 170, 195, 255}, false);

        // Reward Info
        std::string rewardStr = Core::LocalizationManager::Tr("TASK_REWARD_PREFIX") + std::string(" ") + economy.FormatFiat(task.rewardCash);
        UIFrame::DrawTextCustom(rewardStr, rowRec.x + 16.0f, rowRec.y + 54.0f, 14.0f, Color{70, 230, 140, 255}, true);

        // Progress Bar
        float barX = rowRec.x + 290.0f;
        float barY = rowRec.y + 46.0f;
        float barW = rowRec.width - 460.0f;
        float barH = 14.0f;

        if (barW > 60.0f) {
            DrawRectangleRounded(Rectangle{barX, barY, barW, barH}, 0.5f, 4, Color{15, 20, 30, 255});
            
            float progressRatio = std::clamp(static_cast<float>(task.currentProgress / task.targetProgress), 0.0f, 1.0f);
            if (progressRatio > 0.001f) {
                Color progColor = task.isCompleted ? Color{50, 220, 120, 255} : Color{60, 160, 240, 255};
                DrawRectangleRounded(Rectangle{barX, barY, barW * progressRatio, barH}, 0.5f, 4, progColor);
            }
            DrawRectangleRoundedLines(Rectangle{barX, barY, barW, barH}, 0.5f, 4, 1.0f, Color{60, 80, 110, 200});

            // Progress text
            char progressBuf[48];
            if (task.targetProgress <= 10.0) {
                snprintf(progressBuf, sizeof(progressBuf), "%.0f / %.0f", task.currentProgress, task.targetProgress);
            } else {
                snprintf(progressBuf, sizeof(progressBuf), "%.0f / %.0f", task.currentProgress, task.targetProgress);
            }
            float progTextW = UIFrame::MeasureTextCustom(progressBuf, 11.0f, true);
            UIFrame::DrawTextCustom(progressBuf, barX + (barW - progTextW) * 0.5f, barY + 1.0f, 11.0f, Color{240, 240, 240, 255}, true);
        }

        // Action Button / Status
        Rectangle btnRec = m_claimButtons[i].GetBounds();
        if (task.isClaimed) {
            DrawRectangleRounded(btnRec, 0.2f, 4, Color{25, 45, 35, 200});
            DrawRectangleRoundedLines(btnRec, 0.2f, 4, 1.0f, Color{40, 140, 80, 200});
            const char* claimedText = Core::LocalizationManager::Tr("TASK_BTN_CLAIMED");
            float tw = UIFrame::MeasureTextCustom(claimedText, 14.0f, true);
            UIFrame::DrawTextCustom(claimedText, btnRec.x + (btnRec.width - tw) * 0.5f, btnRec.y + 9.0f, 14.0f, Color{70, 210, 130, 255}, true);
        } else if (task.isCompleted) {
            // Glowing animated button
            float pulse = 0.8f + 0.2f * std::sin((float)GetTime() * 6.0f);
            Color claimColor = Color{static_cast<unsigned char>(240 * pulse), static_cast<unsigned char>(180 * pulse), 30, 255};
            DrawRectangleRounded(btnRec, 0.2f, 4, claimColor);
            DrawRectangleRoundedLines(btnRec, 0.2f, 4, 1.5f, Color{255, 240, 120, 255});
            
            const char* claimText = Core::LocalizationManager::Tr("TASK_BTN_CLAIM");
            float tw = UIFrame::MeasureTextCustom(claimText, 14.0f, true);
            UIFrame::DrawTextCustom(claimText, btnRec.x + (btnRec.width - tw) * 0.5f, btnRec.y + 9.0f, 14.0f, Color{20, 20, 20, 255}, true);
        } else {
            // Disabled in-progress
            DrawRectangleRounded(btnRec, 0.2f, 4, Color{30, 38, 52, 180});
            DrawRectangleRoundedLines(btnRec, 0.2f, 4, 1.0f, Color{55, 70, 95, 200});
            const char* progText = Core::LocalizationManager::Tr("TASK_BTN_IN_PROGRESS");
            float tw = UIFrame::MeasureTextCustom(progText, 13.0f, true);
            UIFrame::DrawTextCustom(progText, btnRec.x + (btnRec.width - tw) * 0.5f, btnRec.y + 10.0f, 13.0f, Color{130, 150, 175, 255}, true);
        }
    }

    EndScissorMode();
}

} // namespace Render
