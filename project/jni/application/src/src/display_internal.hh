#ifndef DISPLAY_INTERNAL_HH
#define DISPLAY_INTERNAL_HH

#include "display.hh"
#include "ecl_font.hh"
#include "ecl_video.hh"
#include "enigma.hh"

#include <list>
#include <memory>

namespace display {

class DisplayLayer;
class Model;

typedef ecl::Rect ScreenArea;
typedef ecl::Rect WorldArea;
typedef std::list<Model *> ModelList;

class Window {
public:
    Window() {}
    Window(ScreenArea area) : m_area(std::move(area)) {}

    const ScreenArea &get_area() const { return m_area; }

private:
    ScreenArea m_area;
};

class TextDisplay {
public:
    TextDisplay(ecl::Font &f);

    void set_text(const std::string &t, bool scrolling, double duration = -1);

    void tick(double dtime);
    bool has_changed() const { return changedp; }
    bool has_finished() const { return finishedp; }

    void draw(ecl::GC &gc, const ecl::Rect &r);

private:
    ecl::Rect area;
    std::string text;
    bool changedp, finishedp;
    bool pingpong;
    bool showscroll;
    double xoff;
    double scrollspeed;  // pixels per second
    std::unique_ptr<ecl::Surface> textsurface;
    ecl::Font &font;
    double time, maxtime;
};

class StatusBarImpl : public StatusBar, public Window {
public:
    StatusBarImpl(const ScreenArea &area);
    ~StatusBarImpl();

    bool has_changed() const { return m_changedp; }
    void redraw(ecl::GC &gc, const ScreenArea &r);
    void tick(double dtime);
    void new_world();

    // StatusBar interface.
    void set_time(double time) override;
    void set_inventory(enigma::Player activePlayer, const std::vector<std::string> &modelnames) override;
    void show_text(const std::string &str, bool scrolling, double duration) override;
    void hide_text() override;

    void show_move_counter(bool active) override;
    void setCMode(bool flag) override;
    void setBasicModes(std::string flags) override;

    void set_speed(double speed) override;
    void set_travelled_distance(double distance) override;
    void set_counter(int new_counter) override;

private:
    ScreenArea m_itemarea;
    std::vector<Model *> m_models;
    enigma::Player player;
    bool m_changedp;
    TextDisplay m_textview;

    double m_leveltime;
    bool m_showtime_p;
    int m_counter;
    bool m_showcounter_p;
    bool m_interruptible;  // Current text message may be interrupted
    bool m_text_active;
    bool cMode;  // collision mode flag
    int playerImage;
    double playerImageDuration;
    std::string basicModes;  // set by world on start of level
    int widthDigit[10];
    int widthColon;
    int widthApos;
    int widthQuote;
    int maxWidthDigit;
    bool widthInit;
};

}  // namespace display

#endif
