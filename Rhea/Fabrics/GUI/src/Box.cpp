#include <Rhea/GUI/Box.h>

namespace rhea
{
	void BoxWidget::ready()
	{
		Widget::ready();

		if (!m_alignment)
			m_alignment = std::make_unique<Alignment>(Alignment::Horizontal::CENTER, Alignment::Vertical::CENTER);
	}

	void BoxWidget::onSetPosition()
	{
		m_cur_position = m_position + m_content_position;
		Vector2 child_size;
		Vector2 padding;

		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Widget* w = doCast<Widget>(it->second);
			if (w) {
				Vector2 child_position = m_position;
				if (m_alignment)
				{					
					Vector2 outer_size = w->getOuterSize();
					m_alignment->apply(m_size, outer_size, child_position);
				}
				w->setPosition(child_position);
			}
		}
	}
}
