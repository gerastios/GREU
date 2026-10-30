#pragma once

#include "greucore/include/DetailPtr.h"
#include "greucore/include/Settings.h"
#include "greucore/include/signalUtils.h"
#include <optional>

namespace greu::grylizo {

class Settings : public core::Settings {
public:
    static Settings *instance();

    struct DateFormat {
        static constexpr int ADateW = 0; // mm/dd/yyyy
        static constexpr int EDateW = 1; // dd.mm.yyyy
        static constexpr int SDateW = 2; // yyyy/mm/dd
        static constexpr int iso = 3;    // yyyy-mm-dd
    };

    struct TimeFormat {
        static constexpr int _24 = 0;
        static constexpr int AmPm = 1;
    };

    core::EmptySig &dateFormatChanged() const;
    core::EmptySig &timeFormatChanged() const;

    int dateFormat() const;
    void setDateFormat(int);
    int timeFormat() const;
    void setTimeFormat(int);
    int quadratLength() const;
    void setQuadratLength(int);
    QString lastFileName() const;
    void setLastFileName(QString);
    QString lastFilePath() const;
    void setLastFilePath(QString);
    std::optional<bool> overwriteTableAttribute() const;
    void setOverwriteTableAttribute(std::optional<bool>);

private:
    Settings();
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
