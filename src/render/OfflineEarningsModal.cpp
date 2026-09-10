#include "OfflineEarningsModal.hpp"
#include "UIFrame.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace Render {

OfflineEarningsModal::OfflineEarningsModal()
    : m_isOpen(false),
      m_btnCollect(Rectangle{0, 0, 0, 0}, "HASILATI TOPLA VE DEVAM ET", "", Color{25, 90, 50, 255}, Color{50, 230, 130, 255})
{
}

void OfflineEarningsModal::Show(const Core::OfflineMiningReport& report) {
    m_report = report;
    m_isOpen = true;
}

void OfflineEarningsModal::Close() {
    m_isOpen = false;
}

bool OfflineEarningsModal::IsOpen() const {
    return m_isOpen;
}

void OfflineEarningsModal::Update() {
    if (!m_isOpen) return;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    float modalW = std::min(680.0f, screenW - 40.0f);
    float modalH = std::min(520.0f, screenH - 40.0f);
    float modalX = (screenW - modalW) * 0.5f;
    float modalY = (screenH - modalH) * 0.5f;

    float btnW = modalW - 64.0f;
    float btnH = 50.0f;
    float btnX = modalX + 32.0f;
    float btnY = modalY + modalH - btnH - 24.0f;

    m_btnCollect.SetBounds(Rectangle{btnX, btnY, btnW, btnH});
    if (m_btnCollect.UpdateAndCheckClick()) {
        Close();
    }
}

void OfflineEarningsModal::Draw(const Core::EconomyManager& economy) const {
    if (!m_isOpen) return;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // Koyu arka plan filtresi
    DrawRectangle(0, 0, screenW, screenH, Color{8, 12, 18, 230});

    float modalW = std::min(680.0f, screenW - 40.0f);
    float modalH = std::min(520.0f, screenH - 40.0f);
    float modalX = (screenW - modalW) * 0.5f;
    float modalY = (screenH - modalH) * 0.5f;

    Rectangle modalRec{modalX, modalY, modalW, modalH};

    // Altın parıltılı modal gövdesi
    DrawRectangleRounded(Rectangle{modalRec.x + 4.0f, modalRec.y + 6.0f, modalRec.width, modalRec.height}, 0.04f, 6, Color{0, 0, 0, 120});
    DrawRectangleRounded(modalRec, 0.04f, 6, Color{18, 24, 36, 252});
    DrawRectangleRoundedLines(modalRec, 0.04f, 6, 2.0f, Color{255, 215, 60, 240});

    // 1. Üst Başlık Şeridi
    DrawRectangleRounded(Rectangle{modalX + 16.0f, modalY + 16.0f, modalW - 32.0f, 64.0f}, 0.15f, 4, Color{28, 38, 56, 255});
    DrawRectangleRoundedLines(Rectangle{modalX + 16.0f, modalY + 16.0f, modalW - 32.0f, 64.0f}, 0.15f, 4, 1.2f, Color{255, 205, 50, 200});

    const char* title = "HOS GELDINIZ! CEVRIMDISI MADENCILIK RAPORU";
    float tW = UIFrame::MeasureTextCustom(title, 20.0f, true);
    UIFrame::DrawTextCustom(title, modalX + (modalW - tW) * 0.5f, modalY + 24.0f, 20.0f, Color{255, 220, 60, 255}, true);

    const char* sub = "Siz oyunda yokken tesisleriniz araliksiz calisip kripto kazmaya devam etti!";
    float sW = UIFrame::MeasureTextCustom(sub, 13.0f, false);
    UIFrame::DrawTextCustom(sub, modalX + (modalW - sW) * 0.5f, modalY + 50.0f, 13.0f, Color{180, 200, 225, 255}, false);

    // Süre Hesabı
    int totalSecs = static_cast<int>(m_report.elapsedSeconds);
    int hours = totalSecs / 3600;
    int mins = (totalSecs % 3600) / 60;
    int secs = totalSecs % 60;

    std::ostringstream ssTime;
    if (hours > 0) {
        ssTime << hours << " Saat " << mins << " Dakika";
    } else if (mins > 0) {
        ssTime << mins << " Dakika " << secs << " Saniye";
    } else {
        ssTime << secs << " Saniye";
    }

    // 2. İstatistik Kartları Grid
    float cardX = modalX + 32.0f;
    float cardW = (modalW - 64.0f - 16.0f) * 0.5f;
    float startY = modalY + 96.0f;
    float cardH = 74.0f;
    float gapY = 12.0f;

    // Kart 1: Çevrimdışı Süre
    Rectangle c1Rec{cardX, startY, cardW, cardH};
    DrawRectangleRounded(c1Rec, 0.15f, 4, Color{24, 32, 46, 240});
    DrawRectangleRoundedLines(c1Rec, 0.15f, 4, 1.0f, Color{50, 75, 105, 255});
    UIFrame::DrawTextCustom("GECEN CEVRIMDISI SURE", c1Rec.x + 16.0f, c1Rec.y + 12.0f, 12.0f, Color{140, 165, 195, 255}, true);
    UIFrame::DrawTextCustom(ssTime.str(), c1Rec.x + 16.0f, c1Rec.y + 36.0f, 18.0f, Color{0, 240, 255, 255}, true);

    // Kart 2: Aktif Çalışan Rigler
    Rectangle c2Rec{cardX + cardW + 16.0f, startY, cardW, cardH};
    DrawRectangleRounded(c2Rec, 0.15f, 4, Color{24, 32, 46, 240});
    DrawRectangleRoundedLines(c2Rec, 0.15f, 4, 1.0f, Color{50, 75, 105, 255});
    UIFrame::DrawTextCustom("AKTIF CALISAN RIGLER", c2Rec.x + 16.0f, c2Rec.y + 12.0f, 12.0f, Color{140, 165, 195, 255}, true);
    std::string rigInfo = std::to_string(m_report.totalRigsRunning) + " Rig (Calisiyor)";
    UIFrame::DrawTextCustom(rigInfo, c2Rec.x + 16.0f, c2Rec.y + 36.0f, 18.0f, Color{100, 255, 160, 255}, true);

    // Kart 3: GPU Kazım Hasılatı
    Rectangle c3Rec{cardX, startY + cardH + gapY, cardW, cardH};
    DrawRectangleRounded(c3Rec, 0.15f, 4, Color{24, 32, 46, 240});
    DrawRectangleRoundedLines(c3Rec, 0.15f, 4, 1.0f, Color{50, 75, 105, 255});
    std::string gpuHead = "GPU KAZIMI (" + m_report.gpuCoinSymbol + ")";
    UIFrame::DrawTextCustom(gpuHead, c3Rec.x + 16.0f, c3Rec.y + 12.0f, 12.0f, Color{140, 165, 195, 255}, true);

    std::ostringstream ssGPU;
    ssGPU << "+" << std::fixed << std::setprecision(5) << m_report.gpuCoinsMined << " " << m_report.gpuCoinSymbol;
    UIFrame::DrawTextCustom(ssGPU.str(), c3Rec.x + 16.0f, c3Rec.y + 32.0f, 16.0f, Color{255, 215, 60, 255}, true);
    std::string gpuUSD = "(+" + economy.FormatFiat(m_report.gpuCoinsValueUSD) + ")";
    UIFrame::DrawTextCustom(gpuUSD, c3Rec.x + 16.0f, c3Rec.y + 52.0f, 12.0f, Color{120, 220, 150, 255}, false);

    // Kart 4: CPU Kazım Hasılatı
    Rectangle c4Rec{cardX + cardW + 16.0f, startY + cardH + gapY, cardW, cardH};
    DrawRectangleRounded(c4Rec, 0.15f, 4, Color{24, 32, 46, 240});
    DrawRectangleRoundedLines(c4Rec, 0.15f, 4, 1.0f, Color{50, 75, 105, 255});
    std::string cpuHead = "CPU KAZIMI (" + m_report.cpuCoinSymbol + ")";
    UIFrame::DrawTextCustom(cpuHead, c4Rec.x + 16.0f, c4Rec.y + 12.0f, 12.0f, Color{140, 165, 195, 255}, true);

    std::ostringstream ssCPU;
    ssCPU << "+" << std::fixed << std::setprecision(5) << m_report.cpuCoinsMined << " " << m_report.cpuCoinSymbol;
    UIFrame::DrawTextCustom(ssCPU.str(), c4Rec.x + 16.0f, c4Rec.y + 32.0f, 16.0f, Color{255, 180, 80, 255}, true);
    std::string cpuUSD = "(+" + economy.FormatFiat(m_report.cpuCoinsValueUSD) + ")";
    UIFrame::DrawTextCustom(cpuUSD, c4Rec.x + 16.0f, c4Rec.y + 52.0f, 12.0f, Color{120, 220, 150, 255}, false);

    // Kart 5: Elektrik Maliyeti
    Rectangle c5Rec{cardX, startY + (cardH + gapY) * 2, cardW, cardH};
    DrawRectangleRounded(c5Rec, 0.15f, 4, Color{24, 32, 46, 240});
    DrawRectangleRoundedLines(c5Rec, 0.15f, 4, 1.0f, Color{50, 75, 105, 255});
    UIFrame::DrawTextCustom("ELEKTRIK GIDERI", c5Rec.x + 16.0f, c5Rec.y + 12.0f, 12.0f, Color{140, 165, 195, 255}, true);
    std::string elecStr = "-" + economy.FormatFiat(m_report.electricityCostUSD);
    UIFrame::DrawTextCustom(elecStr, c5Rec.x + 16.0f, c5Rec.y + 36.0f, 18.0f, Color{255, 85, 85, 255}, true);

    // Kart 6: Net Finansal Kar (Vurgulu Büyük Kart)
    Rectangle c6Rec{cardX + cardW + 16.0f, startY + (cardH + gapY) * 2, cardW, cardH};
    Color netBg = (m_report.netEarningsUSD >= 0.0) ? Color{20, 48, 32, 240} : Color{48, 20, 20, 240};
    Color netLine = (m_report.netEarningsUSD >= 0.0) ? Color{0, 230, 120, 255} : Color{255, 75, 75, 255};
    DrawRectangleRounded(c6Rec, 0.15f, 4, netBg);
    DrawRectangleRoundedLines(c6Rec, 0.15f, 4, 1.4f, netLine);
    UIFrame::DrawTextCustom("NET CEVRIMDISI KAZANC", c6Rec.x + 16.0f, c6Rec.y + 12.0f, 12.0f, Color{200, 220, 240, 255}, true);
    std::string netStr = (m_report.netEarningsUSD >= 0.0 ? "+" : "") + economy.FormatFiat(m_report.netEarningsUSD);
    UIFrame::DrawTextCustom(netStr, c6Rec.x + 16.0f, c6Rec.y + 36.0f, 20.0f, netLine, true);

    // 3. Aksiyon Butonu
    m_btnCollect.Draw();
}

} // namespace Render
