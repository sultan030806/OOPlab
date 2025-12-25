#include "shapeditor.h"

// ShapesContainer implementation
void ShapesContainer::add(Shape* shape) {
    shapes.push_back(shape);
}

void ShapesContainer::remove(Shape* shape) {
    auto it = std::find(shapes.begin(), shapes.end(), shape);
    if (it != shapes.end()) {
        delete* it; // Освобождаем память
        shapes.erase(it);
    }
}

std::vector<Shape*> ShapesContainer::get_all() const {
    return shapes;
}

void ShapesContainer::clear_selected() {
    std::vector<QString> selected_types;
    for (Shape* shape : shapes) {
        if (shape->get_selected()) {
            selected_types.push_back(Shape::get_shape_type_name(shape));
        }
    }

    // Освобождаем память удаляемых фигур
    for (Shape* shape : shapes) {
        if (shape->get_selected()) {
            delete shape;
        }
    }

    // Удаляем указатели на выделенные фигуры
    shapes.erase(std::remove_if(shapes.begin(), shapes.end(),
        [](Shape* shape) { return shape->get_selected(); }),
        shapes.end());

    if (!selected_types.empty()) {
        std::cout << "Deleted selected shapes: ";
        for (size_t i = 0; i < selected_types.size(); ++i) {
            std::cout << selected_types[i].toStdString();
            if (i < selected_types.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }
}

size_t ShapesContainer::size() const {
    return shapes.size();
}

ShapesContainer::~ShapesContainer() {
    for (Shape* shape : shapes) {
        delete shape;
    }
    shapes.clear();
}

// Shape implementation
Shape::Shape(int x, int y) : x(x), y(y), width(50), height(50),
color(0, 0, 255), selection_color(255, 0, 0),
line_width(2), selection_line_width(3), is_selected(false) {}

bool Shape::move(int dx, int dy, int canvas_width, int canvas_height) {
    int old_x = x, old_y = y;
    int new_x = x + dx;
    int new_y = y + dy;

    if (new_x >= 0 && new_x <= canvas_width - width &&
        new_y >= 0 && new_y <= canvas_height - height) {
        x = new_x;
        y = new_y;
        std::cout << "Shape " << Shape::get_shape_type_name(this).toStdString()
            << " moved from (" << old_x << ", " << old_y
            << ") to (" << x << ", " << y << ")" << std::endl;
        return true;
    }
    else {
        std::cout << "Attempt to go out of bounds: "
            << Shape::get_shape_type_name(this).toStdString() << " tried to go beyond canvas!" << std::endl;
        return false;
    }
}

bool Shape::resize(int dw, int dh, int canvas_width, int canvas_height) {
    int old_width = width, old_height = height;
    int new_width = width + dw;
    int new_height = height + dh;

    if (new_width > 0 && new_height > 0 &&
        x + new_width <= canvas_width &&
        y + new_height <= canvas_height) {
        width = new_width;
        height = new_height;
        std::cout << "Shape " << Shape::get_shape_type_name(this).toStdString()
            << " resized from (" << old_width << ", " << old_height
            << ") to (" << width << ", " << height << ")" << std::endl;
        return true;
    }
    else {
        std::cout << "Attempt to go out of bounds: failed to resize "
            << Shape::get_shape_type_name(this).toStdString()
            << " by " << dw << ", " << dh << std::endl;
        return false;
    }
}

void Shape::adjust_to_bounds(int canvas_width, int canvas_height) {
    int old_x = x, old_y = y;
    bool adjusted = false;

    if (x + width > canvas_width) {
        x = std::max(0, canvas_width - width);
        adjusted = true;
    }
    if (y + height > canvas_height) {
        y = std::max(0, canvas_height - height);
        adjusted = true;
    }
    if (x < 0) {
        x = 0;
        adjusted = true;
    }
    if (y < 0) {
        y = 0;
        adjusted = true;
    }

    if (adjusted) {
        std::cout << "Shape " << Shape::get_shape_type_name(this).toStdString()
            << " adjusted when canvas resized: from (" << old_x << ", " << old_y
            << ") to (" << x << ", " << y << ")" << std::endl;
    }
}

QString Shape::get_shape_type_name(const Shape* shape) {
    if (dynamic_cast<const Circle*>(shape)) return "Circle";
    if (dynamic_cast<const Rectangle*>(shape)) return "Rectangle";
    if (dynamic_cast<const Square*>(shape)) return "Square";
    if (dynamic_cast<const Ellipse*>(shape)) return "Ellipse";
    if (dynamic_cast<const Triangle*>(shape)) return "Triangle";
    if (dynamic_cast<const Line*>(shape)) return "Line";
    return "Unknown";
}

// Circle implementation
Circle::Circle(int x, int y) : Shape(x, y) {
    selection_color = QColor(255, 69, 0); // Orange
}

void Circle::draw(QPainter& painter) {
    QPen pen(color, line_width);
    painter.setPen(pen);
    painter.drawEllipse(get_x(), get_y(), get_width(), get_height());

    if (is_selected) {
        QPen selection_pen(selection_color, selection_line_width);
        painter.setPen(selection_pen);
        painter.setBrush(QBrush(QColor(0, 0, 0, 0)));
        painter.drawEllipse(get_x() - 3, get_y() - 3, get_width() + 6, get_height() + 6);
    }
}

bool Circle::contains(const QPoint& point) const {
    int center_x = get_x() + get_width() / 2;
    int center_y = get_y() + get_height() / 2;
    int radius = get_width() / 2;
    int dx = point.x() - center_x;
    int dy = point.y() - center_y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

// Rectangle implementation
Rectangle::Rectangle(int x, int y) : Shape(x, y) {
    set_width(80);
    set_height(40);
    selection_color = QColor(0, 255, 255); // Cyan
}

void Rectangle::draw(QPainter& painter) {
    QPen pen(color, line_width);
    painter.setPen(pen);
    painter.drawRect(get_x(), get_y(), get_width(), get_height());

    if (is_selected) {
        QPen selection_pen(selection_color, selection_line_width);
        painter.setPen(selection_pen);
        painter.setBrush(QBrush(QColor(0, 0, 0, 0)));
        painter.drawRect(get_x() - 3, get_y() - 3, get_width() + 6, get_height() + 6);
    }
}

bool Rectangle::contains(const QPoint& point) const {
    return (point.x() >= get_x() && point.x() <= get_x() + get_width() &&
        point.y() >= get_y() && point.y() <= get_y() + get_height());
}

// Square implementation
Square::Square(int x, int y) : Shape(x, y) {
    set_width(50);
    set_height(50);
    selection_color = QColor(255, 0, 255); // Magenta
}

void Square::draw(QPainter& painter) {
    QPen pen(color, line_width);
    painter.setPen(pen);
    painter.drawRect(get_x(), get_y(), get_width(), get_height());

    if (is_selected) {
        QPen selection_pen(selection_color, selection_line_width);
        painter.setPen(selection_pen);
        painter.setBrush(QBrush(QColor(0, 0, 0, 0)));
        painter.drawRect(get_x() - 3, get_y() - 3, get_width() + 6, get_height() + 6);
    }
}

bool Square::contains(const QPoint& point) const {
    return (point.x() >= get_x() && point.x() <= get_x() + get_width() &&
        point.y() >= get_y() && point.y() <= get_y() + get_height());
}

// Ellipse implementation
Ellipse::Ellipse(int x, int y) : Shape(x, y) {
    set_width(70);
    set_height(40);
    selection_color = QColor(128, 128, 0); // Olive
}

void Ellipse::draw(QPainter& painter) {
    QPen pen(color, line_width);
    painter.setPen(pen);
    painter.drawEllipse(get_x(), get_y(), get_width(), get_height());

    if (is_selected) {
        QPen selection_pen(selection_color, selection_line_width);
        painter.setPen(selection_pen);
        painter.setBrush(QBrush(QColor(0, 0, 0, 0)));
        painter.drawEllipse(get_x() - 3, get_y() - 3, get_width() + 6, get_height() + 6);
    }
}

bool Ellipse::contains(const QPoint& point) const {
    int center_x = get_x() + get_width() / 2;
    int center_y = get_y() + get_height() / 2;
    double dx = (point.x() - center_x) / (get_width() / 2.0);
    double dy = (point.y() - center_y) / (get_height() / 2.0);
    return (dx * dx + dy * dy) <= 1.0;
}

// Triangle implementation
Triangle::Triangle(int x, int y) : Shape(x, y) {
    set_width(60);
    set_height(60);
    selection_color = QColor(0, 128, 128); // Teal
}

void Triangle::draw(QPainter& painter) {
    QPolygon polygon;
    polygon << QPoint(get_x() + get_width() / 2, get_y())
        << QPoint(get_x(), get_y() + get_height())
        << QPoint(get_x() + get_width(), get_y() + get_height());

    QPen pen(color, line_width);
    painter.setPen(pen);
    painter.drawPolygon(polygon);

    if (is_selected) {
        QPen selection_pen(selection_color, selection_line_width);
        painter.setPen(selection_pen);
        painter.setBrush(QBrush(QColor(0, 0, 0, 0)));
        painter.drawPolygon(polygon);
    }
}

bool Triangle::contains(const QPoint& point) const {
    return (point.x() >= get_x() && point.x() <= get_x() + get_width() &&
        point.y() >= get_y() && point.y() <= get_y() + get_height());
}

// Line implementation
Line::Line(int x, int y) : Shape(x, y) {
    set_width(80);
    set_height(2);
    selection_color = QColor(128, 0, 128); // Purple
    line_width = 3;
    selection_line_width = 4;
}

void Line::draw(QPainter& painter) {
    QPen pen(color, line_width);
    painter.setPen(pen);
    painter.drawLine(get_x(), get_y(), get_x() + get_width(), get_y() + get_height());

    if (is_selected) {
        QPen selection_pen(selection_color, selection_line_width);
        painter.setPen(selection_pen);
        painter.drawRect(get_x() - 3, get_y() - 3, get_width() + 6, get_height() + 6);
    }
}

bool Line::contains(const QPoint& point) const {
    return (point.x() >= get_x() - 5 && point.x() <= get_x() + get_width() + 5 &&
        point.y() >= get_y() - 5 && point.y() <= get_y() + get_height() + 5);
}

// CanvasWidget implementation
CanvasWidget::CanvasWidget(QWidget* parent) : QWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
    current_shape_type = "";
}

void CanvasWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw non-selected shapes first
    for (Shape* shape : shapes_container.get_all()) {
        if (!shape->get_selected()) {
            shape->draw(painter);
        }
    }

    // Draw selected shapes on top
    for (Shape* shape : shapes_container.get_all()) {
        if (shape->get_selected()) {
            shape->draw(painter);
        }
    }
}

void CanvasWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        int x = event->pos().x();
        int y = event->pos().y();

        // Find all shapes at click point
        std::vector<Shape*> shapes_at_point;
        for (Shape* shape : shapes_container.get_all()) {
            if (shape->contains(event->pos())) {
                shapes_at_point.push_back(shape);
            }
        }

        if (!shapes_at_point.empty()) {
            // Ctrl+click - toggle selection
            if (event->modifiers() & Qt::ControlModifier) {
                for (Shape* shape : shapes_at_point) {
                    shape->set_selected(!shape->get_selected());
                }
            }
            else {
                // Regular click - deselect all and select shapes at point
                for (Shape* shape : shapes_container.get_all()) {
                    shape->set_selected(false);
                }
                for (Shape* shape : shapes_at_point) {
                    shape->set_selected(true);
                }
            }

            std::vector<QString> selected_types;
            for (Shape* shape : shapes_at_point) {
                selected_types.push_back(Shape::get_shape_type_name(shape));
            }

            QString action = (event->modifiers() & Qt::ControlModifier) ?
                "toggled" : "selected";
            std::cout << "Selection " << action.toStdString() << " for shapes: ";
            for (size_t i = 0; i < selected_types.size(); ++i) {
                std::cout << selected_types[i].toStdString();
                if (i < selected_types.size() - 1) std::cout << ", ";
            }
            std::cout << std::endl;
        }
        else {
            // Click on empty space - deselect all
            for (Shape* shape : shapes_container.get_all()) {
                shape->set_selected(false);
            }
            std::cout << "Deselected all shapes" << std::endl;

            // Create new shape if type is selected
            if (!current_shape_type.isEmpty()) {
                Shape* new_shape = nullptr;

                if (current_shape_type == "rectangle") {
                    new_shape = new Rectangle(x, y);
                }
                else if (current_shape_type == "circle") {
                    new_shape = new Circle(x, y);
                }
                else if (current_shape_type == "square") {
                    new_shape = new Square(x, y);
                }
                else if (current_shape_type == "ellipse") {
                    new_shape = new Ellipse(x, y);
                }
                else if (current_shape_type == "triangle") {
                    new_shape = new Triangle(x, y);
                }
                else if (current_shape_type == "line") {
                    new_shape = new Line(x, y);
                }

                if (new_shape) {
                    shapes_container.add(new_shape);
                    std::cout << "Created shape "
                        << Shape::get_shape_type_name(new_shape).toStdString()
                        << " at position (" << x << ", " << y
                        << ") with size (" << new_shape->get_width() << ", "
                        << new_shape->get_height() << ")" << std::endl;
                }
            }
        }

        update();
    }
}

void CanvasWidget::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Delete) {
        shapes_container.clear_selected();
        update();
    }
    else if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right ||
        event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) {

        if (event->modifiers() & Qt::ShiftModifier) {
            // Resize with Shift
            int dw = 0, dh = 0;
            if (event->key() == Qt::Key_Left) dw = -5;
            else if (event->key() == Qt::Key_Right) dw = 5;
            else if (event->key() == Qt::Key_Up) dh = -5;
            else if (event->key() == Qt::Key_Down) dh = 5;

            for (Shape* shape : shapes_container.get_all()) {
                if (shape->get_selected()) {
                    shape->resize(dw, dh, width(), height());
                }
            }
            update();
        }
        else {
            // Move without Shift
            int dx = 0, dy = 0;
            if (event->key() == Qt::Key_Left) dx = -5;
            else if (event->key() == Qt::Key_Right) dx = 5;
            else if (event->key() == Qt::Key_Up) dy = -5;
            else if (event->key() == Qt::Key_Down) dy = 5;

            for (Shape* shape : shapes_container.get_all()) {
                if (shape->get_selected()) {
                    shape->move(dx, dy, width(), height());
                }
            }
            update();
        }
    }
}

void CanvasWidget::resizeEvent(QResizeEvent* event) {
    QSize old_size = event->oldSize();
    QSize new_size = event->size();
    std::cout << "Canvas changed from " << old_size.width() << "x" << old_size.height()
        << " to " << new_size.width() << "x" << new_size.height() << std::endl;

    QWidget::resizeEvent(event);

    for (Shape* shape : shapes_container.get_all()) {
        shape->adjust_to_bounds(width(), height());
    }
    update();
}

void CanvasWidget::set_current_shape_type(const QString& shape_type) {
    current_shape_type = shape_type;
}

void CanvasWidget::change_selected_shapes_color(const QColor& color) {
    std::vector<QString> changed_types;
    for (Shape* shape : shapes_container.get_all()) {
        if (shape->get_selected()) {
            shape->set_color(color);
            changed_types.push_back(Shape::get_shape_type_name(shape));
        }
    }

    if (!changed_types.empty()) {
        std::cout << "Changed color for shapes: ";
        for (size_t i = 0; i < changed_types.size(); ++i) {
            std::cout << changed_types[i].toStdString();
            if (i < changed_types.size() - 1) std::cout << ", ";
        }
        std::cout << " to " << color.name().toStdString() << std::endl;
    }
}

// ShapeEditor implementation
ShapeEditor::ShapeEditor(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Vector Graphics Editor");
    resize(800, 600);

    canvas = new CanvasWidget(this);
    setCentralWidget(canvas);

    create_menu();
    create_toolbar();
}

void ShapeEditor::create_menu() {
    QMenuBar* menu_bar = menuBar();

    // Shapes menu
    QMenu* shapes_menu = menu_bar->addMenu("Shapes");

    QAction* rectangle_action = new QAction("Rectangle", this);
    QAction* circle_action = new QAction("Circle", this);
    QAction* square_action = new QAction("Square", this);
    QAction* ellipse_action = new QAction("Ellipse", this);
    QAction* triangle_action = new QAction("Triangle", this);
    QAction* line_action = new QAction("Line", this);

    shapes_menu->addAction(rectangle_action);
    shapes_menu->addAction(circle_action);
    shapes_menu->addAction(square_action);
    shapes_menu->addAction(ellipse_action);
    shapes_menu->addAction(triangle_action);
    shapes_menu->addAction(line_action);

    connect(rectangle_action, &QAction::triggered, this, [this]() { set_shape_type("rectangle"); });
    connect(circle_action, &QAction::triggered, this, [this]() { set_shape_type("circle"); });
    connect(square_action, &QAction::triggered, this, [this]() { set_shape_type("square"); });
    connect(ellipse_action, &QAction::triggered, this, [this]() { set_shape_type("ellipse"); });
    connect(triangle_action, &QAction::triggered, this, [this]() { set_shape_type("triangle"); });
    connect(line_action, &QAction::triggered, this, [this]() { set_shape_type("line"); });

    // Color menu
    QMenu* color_menu = menu_bar->addMenu("Color");
    QAction* change_color_action = new QAction("Change selected color", this);
    connect(change_color_action, &QAction::triggered, this, &ShapeEditor::change_color);
    color_menu->addAction(change_color_action);
}

void ShapeEditor::create_toolbar() {
    QToolBar* toolbar = new QToolBar("Toolbar", this);
    addToolBar(toolbar);

    std::vector<std::pair<QString, QString>> shape_actions = {
        {"Rectangle", "rectangle"},
        {"Circle", "circle"},
        {"Square", "square"},
        {"Ellipse", "ellipse"},
        {"Triangle", "triangle"},
        {"Line", "line"}
    };

    for (const auto& [text, shape_type] : shape_actions) {
        QAction* action = new QAction(text, this);
        connect(action, &QAction::triggered, this, [this, shape_type]() { set_shape_type(shape_type); });
        toolbar->addAction(action);
    }
}

void ShapeEditor::set_shape_type(const QString& shape_type) {
    canvas->set_current_shape_type(shape_type);
    std::cout << "Selected shape: " << shape_type.toStdString() << std::endl;
}

void ShapeEditor::change_color() {
    QColor color = QColorDialog::getColor();
    if (color.isValid()) {
        canvas->change_selected_shapes_color(color);
        canvas->update();
    }
}
