#ifndef MAP_H
#define MAP_H

namespace RadialMap {
    class Segment;

    class Map : public QPixmap
    {
        public:
            Map();
            ~Map();

            void make(const Directory *, bool = false);
            bool resize(const QRect&);

            bool isNull() const { return (m_signature == 0); }
            void invalidate(const bool);

            friend class Builder;
            friend class Widget;

        private:
            void paint(uint = 1);
            void aaPaint();
            void colorise();
            void setRingBreadth();

            Chain<Segment> *m_signature;

            QRect   m_rect;
            uint    m_ringBreadth;  ///ring breadth
            uint    m_innerRadius;  ///radius of inner circle
            uint    m_visibleDepth; ///visible level depth of system
            QString m_centerText;

            uint MAP_2MARGIN;
    };
}

#endif