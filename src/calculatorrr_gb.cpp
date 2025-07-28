#include <gtk/gtk.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <stdexcept>

class Calculator {
private:
    GtkWidget *window;
    GtkWidget *display;
    GtkEntryBuffer *display_buffer;
    std::string current_input;
    std::string stored_value;
    std::string operation;
    bool should_clear_display;
    bool last_was_equals;

public:
    Calculator() : should_clear_display(false), last_was_equals(false) {
        // Initialize GTK
        gtk_init();
        
        // Create main window
        window = gtk_window_new();
        gtk_window_set_title(GTK_WINDOW(window), "calculatorrr");
        gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);
        gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
        
        // Connect destroy signal
        g_signal_connect(window, "destroy", G_CALLBACK(gtk_window_destroy), NULL);
        
        // Create main container
        GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        gtk_widget_set_margin_start(vbox, 10);
        gtk_widget_set_margin_end(vbox, 10);
        gtk_widget_set_margin_top(vbox, 10);
        gtk_widget_set_margin_bottom(vbox, 10);
        gtk_window_set_child(GTK_WINDOW(window), vbox);
        
        // Create display using GtkText
        display = gtk_text_new();
        display_buffer = gtk_entry_buffer_new("0", -1);
        gtk_text_set_buffer(GTK_TEXT(display), display_buffer);
        gtk_editable_set_editable(GTK_EDITABLE(display), FALSE);
        gtk_widget_set_size_request(display, -1, 50);
        gtk_widget_set_halign(display, GTK_ALIGN_FILL);
        gtk_widget_add_css_class(display, "calculator-display");
        
        gtk_box_append(GTK_BOX(vbox), display);
        
        // Create button grid
        GtkWidget *grid = gtk_grid_new();
        gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
        gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
        gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
        gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
        gtk_box_append(GTK_BOX(vbox), grid);
        
        // Button layout
        const char* button_labels[5][4] = {
            {"C", "±", "%", "÷"},
            {"7", "8", "9", "×"},
            {"4", "5", "6", "−"},
            {"1", "2", "3", "+"},
            {"0", ".", "=", "="}
        };
        
        // Create buttons
        for (int row = 0; row < 5; row++) {
            for (int col = 0; col < 4; col++) {
                if (row == 4 && col == 3) continue; // Skip duplicate equals
                
                GtkWidget *button = gtk_button_new_with_label(button_labels[row][col]);
                gtk_widget_set_size_request(button, 60, 60);
                
                // Special styling for operator buttons
                if ((row == 0 && col >= 2) || (row >= 1 && row <= 3 && col == 3) || 
                    (row == 4 && col == 2)) {
                    gtk_widget_add_css_class(button, "operator-button");
                }
                
                // Special width for 0 button
                if (row == 4 && col == 0) {
                    gtk_grid_attach(GTK_GRID(grid), button, col, row, 2, 1);
                } else {
                    int actual_col = (row == 4 && col >= 1) ? col + 1 : col;
                    gtk_grid_attach(GTK_GRID(grid), button, actual_col, row, 1, 1);
                }
                
                // Connect button signals
                g_signal_connect(button, "clicked", 
                    G_CALLBACK(+[](GtkWidget *widget, gpointer data) {
                        Calculator *calc = static_cast<Calculator*>(data);
                        const char *label = gtk_button_get_label(GTK_BUTTON(widget));
                        calc->on_button_clicked(label);
                    }), this);
            }
        }
        
        // Add Gruvbox Dark Hard theme styling
        GtkCssProvider *css_provider = gtk_css_provider_new();
        gtk_css_provider_load_from_string(css_provider, 
            "window { "
            "  background-color: #1d2021; "
            "} "
            "box { "
            "  background-color: #1d2021; "
            "} "
            ".calculator-display { "
            "  font-family: 'JetBrains Mono', 'Fira Code', 'Consolas', monospace; "
            "  font-size: 20px; "
            "  font-weight: bold; "
            "  background-color: #282828; "
            "  color: #ebdbb2; "
            "  padding: 15px; "
            "  text-align: right; "
            "  border: 2px solid #3c3836; "
            "  border-radius: 8px; "
            "  margin-bottom: 10px; "
            "} "
            "button { "
            "  font-family: 'JetBrains Mono', 'Fira Code', 'Consolas', monospace; "
            "  font-size: 16px; "
            "  font-weight: bold; "
            "  min-height: 55px; "
            "  min-width: 65px; "
            "  background-color: #504945; "
            "  color: #ebdbb2; "
            "  border: 2px solid #665c54; "
            "  border-radius: 6px; "
            "  transition: all 0.1s ease; "
            "} "
            "button:hover { "
            "  background-color: #665c54; "
            "  border-color: #7c6f64; "
            "} "
            "button:active { "
            "  background-color: #3c3836; "
            "  border-color: #504945; "
            "} "
            ".operator-button { "
            "  background-color: #d79921; "
            "  color: #1d2021; "
            "  border-color: #fabd2f; "
            "} "
            ".operator-button:hover { "
            "  background-color: #fabd2f; "
            "  border-color: #b57614; "
            "} "
            ".operator-button:active { "
            "  background-color: #b57614; "
            "  border-color: #d79921; "
            "} "
            "grid { "
            "  background-color: #1d2021; "
            "}");
        
        gtk_style_context_add_provider_for_display(gdk_display_get_default(),
            GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        
        // Show window
        gtk_window_present(GTK_WINDOW(window));
    }
    
    void on_button_clicked(const char *label) {
        std::string btn = label;
        
        if (btn >= "0" && btn <= "9") {
            handle_number(btn);
        } else if (btn == ".") {
            handle_decimal();
        } else if (btn == "C") {
            clear();
        } else if (btn == "±") {
            toggle_sign();
        } else if (btn == "%") {
            handle_percentage();
        } else if (btn == "+" || btn == "−" || btn == "×" || btn == "÷") {
            handle_operation(btn);
        } else if (btn == "=") {
            calculate();
        }
    }
    
    void handle_number(const std::string &num) {
        if (should_clear_display || last_was_equals) {
            current_input = "";
            should_clear_display = false;
            last_was_equals = false;
        }
        
        if (current_input == "0") {
            current_input = num;
        } else {
            current_input += num;
        }
        update_display();
    }
    
    void handle_decimal() {
        if (should_clear_display || last_was_equals) {
            current_input = "0";
            should_clear_display = false;
            last_was_equals = false;
        }
        
        if (current_input.find('.') == std::string::npos) {
            if (current_input.empty()) {
                current_input = "0";
            }
            current_input += ".";
            update_display();
        }
    }
    
    void clear() {
        current_input = "";
        stored_value = "";
        operation = "";
        should_clear_display = false;
        last_was_equals = false;
        gtk_entry_buffer_set_text(display_buffer, "0", -1);
    }
    
    void toggle_sign() {
        if (!current_input.empty() && current_input != "0") {
            if (current_input[0] == '-') {
                current_input = current_input.substr(1);
            } else {
                current_input = "-" + current_input;
            }
            update_display();
        }
    }
    
    void handle_percentage() {
        if (!current_input.empty()) {
            try {
                double value = std::stod(current_input);
                value /= 100.0;
                current_input = format_number(value);
                update_display();
            } catch (...) {
                gtk_entry_buffer_set_text(display_buffer, "Error", -1);
            }
        }
    }
    
    void handle_operation(const std::string &op) {
        if (!current_input.empty() && !operation.empty() && !should_clear_display) {
            calculate();
        }
        
        if (!current_input.empty()) {
            stored_value = current_input;
        }
        
        operation = op;
        should_clear_display = true;
        last_was_equals = false;
    }
    
    void calculate() {
        if (stored_value.empty() || operation.empty() || current_input.empty()) {
            return;
        }
        
        try {
            double val1 = std::stod(stored_value);
            double val2 = std::stod(current_input);
            double result = 0.0;
            
            if (operation == "+") {
                result = val1 + val2;
            } else if (operation == "−") {
                result = val1 - val2;
            } else if (operation == "×") {
                result = val1 * val2;
            } else if (operation == "÷") {
                if (val2 == 0) {
                    gtk_entry_buffer_set_text(display_buffer, "Error", -1);
                    return;
                }
                result = val1 / val2;
            }
            
            current_input = format_number(result);
            stored_value = "";
            operation = "";
            should_clear_display = false;
            last_was_equals = true;
            update_display();
            
        } catch (...) {
            gtk_entry_buffer_set_text(display_buffer, "Error", -1);
        }
    }
    
    std::string format_number(double num) {
        // Handle special cases
        if (std::isnan(num) || std::isinf(num)) {
            return "Error";
        }
        
        // Check if it's effectively an integer
        if (std::abs(num - std::round(num)) < 1e-10) {
            return std::to_string(static_cast<long long>(std::round(num)));
        }
        
        // Format with appropriate precision
        std::ostringstream oss;
        oss << std::setprecision(12) << num;
        std::string result = oss.str();
        
        // Remove trailing zeros after decimal point
        if (result.find('.') != std::string::npos) {
            result.erase(result.find_last_not_of('0') + 1, std::string::npos);
            result.erase(result.find_last_not_of('.') + 1, std::string::npos);
        }
        
        return result;
    }
    
    void update_display() {
        std::string display_text = current_input.empty() ? "0" : current_input;
        gtk_entry_buffer_set_text(display_buffer, display_text.c_str(), -1);
    }
    
    void run() {
        // Create GMainLoop and run
        GMainLoop *loop = g_main_loop_new(NULL, FALSE);
        
        // Connect window destroy to quit the main loop
        g_signal_connect(window, "destroy", 
            G_CALLBACK(+[](GtkWidget *widget, gpointer data) {
                g_main_loop_quit(static_cast<GMainLoop*>(data));
            }), loop);
        
        g_main_loop_run(loop);
        g_main_loop_unref(loop);
    }
};

int main(int argc, char *argv[]) {
    Calculator calc;
    calc.run();
    return 0;
}
