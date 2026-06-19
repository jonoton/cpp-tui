#include <cassert>
#include <iostream>

#include "cpptui.hpp"

using namespace cpptui;

int main() {
  // Test Case 1: Flexible containers grow to parent (auto_shrink = false by
  // default)
  {
    auto outer_border = std::make_shared<Border>(BorderStyle::Double);
    auto vertical_layout = std::make_shared<Vertical>();
    auto spacer = std::make_shared<VerticalSpacer>(1);
    auto inner_border = std::make_shared<Border>(BorderStyle::Rounded);

    auto table = std::make_shared<TableScrollable>();
    table->fixed_height = 8;
    table->fixed_width = 15;

    inner_border->add(table);
    vertical_layout->add(spacer);
    vertical_layout->add(inner_border);
    outer_border->add(vertical_layout);

    // Verify default auto_shrink value
    assert(outer_border->auto_shrink == false);
    assert(vertical_layout->auto_shrink == false);
    assert(inner_border->auto_shrink == false);

    // Trigger update_responsive
    outer_border->update_responsive();

    // With auto_shrink = false, everything remains flexible (fixed size = 0)
    std::cout << "--- Flexible Sizing Verification (auto_shrink = false) ---"
              << std::endl;
    std::cout << "Outer Border: fixed_height=" << outer_border->fixed_height
              << " (expected 0), fixed_width=" << outer_border->fixed_width
              << " (expected 0)" << std::endl;
    assert(outer_border->fixed_height == 0);
    assert(outer_border->fixed_width == 0);
    assert(vertical_layout->fixed_height == 0);
    assert(vertical_layout->fixed_width == 0);
    assert(inner_border->fixed_height == 0);
    assert(inner_border->fixed_width == 0);

    // Outer border gets allocated size by the parent (e.g. height=13, width=19)
    outer_border->x = 0;
    outer_border->y = 0;
    outer_border->width = 19;
    outer_border->height = 13;
    outer_border->layout();

    // Layout should allocate flexible container heights to fill parent while
    // child respects its fixed height (8)
    std::cout << "Outer Border: height=" << outer_border->height
              << ", width=" << outer_border->width << std::endl;
    std::cout << "Vertical Layout: height=" << vertical_layout->height
              << " (expected 11), width=" << vertical_layout->width
              << " (expected 17)" << std::endl;
    std::cout << "VerticalSpacer: height=" << spacer->height
              << " (expected 1), width=" << spacer->width << " (expected 17)"
              << std::endl;
    std::cout << "Inner Border: height=" << inner_border->height
              << " (expected 10), width=" << inner_border->width
              << " (expected 17)" << std::endl;
    std::cout << "TableScrollable: height=" << table->height
              << " (expected 8), width=" << table->width << " (expected 15)"
              << std::endl;

    assert(vertical_layout->height == 11);
    assert(vertical_layout->width == 17);
    assert(spacer->height == 1);
    assert(inner_border->height == 10);
    assert(inner_border->width == 17);
    assert(table->height == 8);
    assert(table->width == 15);
  }

  // Test Case 2: Auto-shrink containers wrap their child constraints
  // (auto_shrink = true)
  {
    auto outer_border = std::make_shared<Border>(BorderStyle::Double);
    auto vertical_layout = std::make_shared<Vertical>();
    auto spacer = std::make_shared<VerticalSpacer>(1);
    auto inner_border = std::make_shared<Border>(BorderStyle::Rounded);

    auto table = std::make_shared<TableScrollable>();
    table->fixed_height = 8;
    table->fixed_width = 15;

    inner_border->add(table);
    vertical_layout->add(spacer);
    vertical_layout->add(inner_border);
    outer_border->add(vertical_layout);

    // Enable auto-shrink on all wrappers and layouts
    outer_border->auto_shrink = true;
    vertical_layout->auto_shrink = true;
    inner_border->auto_shrink = true;

    // Trigger update_responsive
    outer_border->update_responsive();

    // Verify propagated fixed sizes
    std::cout << "\n--- Shrink Sizing Verification (auto_shrink = true) ---"
              << std::endl;
    std::cout << "Inner Border: fixed_height=" << inner_border->fixed_height
              << " (expected 10), fixed_width=" << inner_border->fixed_width
              << " (expected 17)" << std::endl;
    std::cout << "Vertical Layout: fixed_height="
              << vertical_layout->fixed_height
              << " (expected 11), fixed_width=" << vertical_layout->fixed_width
              << " (expected 17)" << std::endl;
    std::cout << "Outer Border: fixed_height=" << outer_border->fixed_height
              << " (expected 13), fixed_width=" << outer_border->fixed_width
              << " (expected 19)" << std::endl;

    assert(inner_border->fixed_height == 10);
    assert(inner_border->fixed_width == 17);
    assert(vertical_layout->fixed_height == 11);
    assert(vertical_layout->fixed_width == 17);
    assert(outer_border->fixed_height == 13);
    assert(outer_border->fixed_width == 19);
  }

  // Test Case 3: Border -> Scrollable Vertical -> Spacer + Border ->
  // TableScrollable
  {
    auto outer_border = std::make_shared<Border>(BorderStyle::Double);
    auto scrollable_vertical = std::make_shared<ScrollableVertical>();
    auto spacer = std::make_shared<VerticalSpacer>(1);
    auto inner_border = std::make_shared<Border>(BorderStyle::Rounded);

    auto table = std::make_shared<TableScrollable>();
    table->fixed_height = 8;
    table->fixed_width = 15;

    inner_border->add(table);
    scrollable_vertical->add(spacer);
    scrollable_vertical->add(inner_border);
    outer_border->add(scrollable_vertical);

    // We enable auto_shrink on inner_border so it adapts to the table,
    // but keep scrollable_vertical and outer_border flexible.
    inner_border->auto_shrink = true;

    // Trigger update_responsive
    outer_border->update_responsive();

    // Verify default auto_shrink values and propagated inner_border height
    std::cout << "\n--- ScrollableVertical Sizing Verification (auto_shrink = "
                 "false) ---"
              << std::endl;
    assert(outer_border->auto_shrink == false);
    assert(scrollable_vertical->auto_shrink == false);
    assert(inner_border->auto_shrink == true);

    std::cout << "Inner Border fixed size: fixed_height="
              << inner_border->fixed_height
              << " (expected 10), fixed_width=" << inner_border->fixed_width
              << " (expected 17)" << std::endl;
    assert(inner_border->fixed_height == 10);
    assert(inner_border->fixed_width == 17);
    assert(scrollable_vertical->fixed_height == 0);
    assert(scrollable_vertical->fixed_width == 0);
    assert(outer_border->fixed_height == 0);
    assert(outer_border->fixed_width == 0);

    // Set outer border size simulating parent container layout
    outer_border->x = 0;
    outer_border->y = 0;
    outer_border->width = 19;
    outer_border->height = 13;
    outer_border->layout();

    std::cout << "Outer Border: height=" << outer_border->height
              << ", width=" << outer_border->width << std::endl;
    std::cout << "ScrollableVertical: height=" << scrollable_vertical->height
              << " (expected 11), width=" << scrollable_vertical->width
              << " (expected 17)" << std::endl;
    std::cout << "VerticalSpacer: height=" << spacer->height
              << " (expected 1), width=" << spacer->width << " (expected 17)"
              << std::endl;
    std::cout << "Inner Border: height=" << inner_border->height
              << " (expected 10), width=" << inner_border->width
              << " (expected 17)" << std::endl;
    std::cout << "TableScrollable: height=" << table->height
              << " (expected 8), width=" << table->width << " (expected 15)"
              << std::endl;

    assert(scrollable_vertical->height == 11);
    assert(scrollable_vertical->width == 17);
    assert(spacer->height == 1);
    assert(inner_border->height == 10);
    assert(inner_border->width == 17);
    assert(table->height == 8);
    assert(table->width == 15);

    // Now enable auto-shrink on everything
    outer_border->auto_shrink = true;
    scrollable_vertical->auto_shrink = true;

    // Trigger update_responsive
    outer_border->update_responsive();

    // Verify propagated fixed sizes when everything shrinks
    std::cout << "\n--- ScrollableVertical Sizing Verification (auto_shrink = "
                 "true) ---"
              << std::endl;
    std::cout << "Inner Border: fixed_height=" << inner_border->fixed_height
              << " (expected 10), fixed_width=" << inner_border->fixed_width
              << " (expected 17)" << std::endl;
    std::cout << "ScrollableVertical: fixed_height="
              << scrollable_vertical->fixed_height
              << " (expected 11), fixed_width="
              << scrollable_vertical->fixed_width << " (expected 17)"
              << std::endl;
    std::cout << "Outer Border: fixed_height=" << outer_border->fixed_height
              << " (expected 13), fixed_width=" << outer_border->fixed_width
              << " (expected 19)" << std::endl;

    assert(inner_border->fixed_height == 10);
    assert(inner_border->fixed_width == 17);
    assert(scrollable_vertical->fixed_height == 11);
    assert(scrollable_vertical->fixed_width == 17);
    assert(outer_border->fixed_height == 13);
    assert(outer_border->fixed_width == 19);
  }

  // Test Case 4: Explicit fixed size preservation when auto_shrink is false
  {
    auto border = std::make_shared<Border>(BorderStyle::Single);
    border->fixed_height = 2;
    border->fixed_width = 2;

    border->update_responsive();

    std::cout << "--- Explicit Size Preservation Verification ---" << std::endl;
    std::cout << "Border: fixed_height=" << border->fixed_height
              << " (expected 2), fixed_width=" << border->fixed_width
              << " (expected 2)" << std::endl;
    assert(border->fixed_height == 2);
    assert(border->fixed_width == 2);
  }

  // Test Case 5: Vertical with min/max height on flex children
  {
    auto vertical_layout = std::make_shared<Vertical>();
    auto child1 = std::make_shared<VerticalSpacer>(1);  // flex
    child1->min_height = 4;
    auto child2 = std::make_shared<VerticalSpacer>(1);  // flex
    child2->max_height = 2;
    auto child3 = std::make_shared<VerticalSpacer>(1);  // flex

    // Clear child height request so they are treated as flex
    child1->fixed_height = 0;
    child2->fixed_height = 0;
    child3->fixed_height = 0;

    vertical_layout->add(child1);
    vertical_layout->add(child2);
    vertical_layout->add(child3);

    vertical_layout->x = 0;
    vertical_layout->y = 0;
    vertical_layout->width = 10;
    vertical_layout->height = 10;
    vertical_layout->layout();

    std::cout
        << "--- Test Case 5: Flex distribution with min/max constraints ---"
        << std::endl;
    std::cout << "Child 1 Height: " << child1->height << " (expected 4)"
              << std::endl;
    std::cout << "Child 2 Height: " << child2->height << " (expected 2)"
              << std::endl;
    std::cout << "Child 3 Height: " << child3->height << " (expected 4)"
              << std::endl;

    assert(child1->height == 4);
    assert(child2->height == 2);
    assert(child3->height == 4);
  }

  // Test Case 6: Border with min/max constraints on child
  {
    auto border = std::make_shared<Border>(BorderStyle::Single);
    auto child = std::make_shared<Vertical>();
    child->min_width = 15;
    child->max_width = 16;
    child->min_height = 5;
    child->max_height = 6;

    border->add(child);
    border->x = 0;
    border->y = 0;
    border->width = 20;
    border->height = 10;
    border->layout();

    std::cout << "--- Test Case 6: Border with min/max child constraints ---"
              << std::endl;
    std::cout << "Child width: " << child->width << " (expected 16)"
              << std::endl;
    std::cout << "Child height: " << child->height << " (expected 6)"
              << std::endl;

    assert(child->width == 16);
    assert(child->height == 6);
  }

  // Test Case 7: Auto-shrink propagation respecting min/max
  {
    auto border = std::make_shared<Border>(BorderStyle::Single);
    border->auto_shrink = true;
    border->min_width = 10;
    border->max_width = 20;
    border->min_height = 8;
    border->max_height = 12;

    auto child = std::make_shared<VerticalSpacer>(1);
    child->fixed_width = 5;
    child->fixed_height = 5;

    border->add(child);
    border->update_responsive();

    std::cout << "--- Test Case 7: Auto-shrink propagation with min/max ---"
              << std::endl;
    std::cout << "Border fixed_width: " << border->fixed_width
              << " (expected 10 due to min_width)" << std::endl;
    std::cout << "Border fixed_height: " << border->fixed_height
              << " (expected 8 due to min_height)" << std::endl;

    assert(border->fixed_width == 10);
    assert(border->fixed_height == 8);

    // Let's verify revert_axis works: if we turn off auto-shrink, does it clear
    // them?
    border->auto_shrink = false;
    border->update_responsive();
    std::cout << "Border fixed_width after disable auto-shrink: "
              << border->fixed_width << " (expected 0)" << std::endl;
    assert(border->fixed_width == 0);
  }

  // Test Case 8: Edge case: min > max (min wins)
  {
    std::cout << "--- Test Case 8: min > max constraint resolution ---"
              << std::endl;
    int clamped = Widget::clamp_size(5, 6, 4);
    std::cout << "Clamped size (val=5, min=6, max=4): " << clamped
              << " (expected 6)" << std::endl;
    assert(clamped == 6);
  }

  // Test Case 9: Edge case: min > available space
  {
    auto vertical_layout = std::make_shared<Vertical>();
    auto child1 = std::make_shared<VerticalSpacer>(1);
    child1->min_height = 12;
    child1->fixed_height = 0;

    vertical_layout->add(child1);
    vertical_layout->x = 0;
    vertical_layout->y = 0;
    vertical_layout->width = 10;
    vertical_layout->height = 10;
    vertical_layout->layout();

    std::cout << "--- Test Case 9: min > available space ---" << std::endl;
    std::cout << "Child 1 Height after clipping: " << child1->height
              << " (expected 10)" << std::endl;
    assert(child1->height == 10);
  }

  std::cout << "\nAll nested layout propagation and grow/shrink resolution "
               "tests passed successfully!"
            << std::endl;
  return 0;
}
