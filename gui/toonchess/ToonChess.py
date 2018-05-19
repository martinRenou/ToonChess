from traits.api import Button, Color, HasStrictTraits, Str, Range, Enum
from traitsui.api import Item, UItem, RangeEditor, spring, VGroup, HGroup, View
from pyface.image_resource import ImageResource

from get_share_path import get_share_path


class ToonChess(HasStrictTraits):
    resolution = Enum('fullscreen', '800/600', '1024/000', '1920/1600')
    shadows = Enum('high', 'low', 'very low')
    antialiasing = Enum('high', 'low', 'none')

    difficulty = Enum('very high', 'high', 'normal', 'easy')

    user_color = Color()
    ai_color = Color()

    play_button = Button("Play")
    reset_button = Button("Reset default")

    traits_view = View(
        VGroup(
            HGroup(
                Item('resolution'),
                Item('shadows'),
                Item('antialiasing'),
                label='Graphical settings',
                show_border=True
            ),
            HGroup(
                Item('difficulty'),
                show_border=True
            ),
            HGroup(
                Item('user_color', style='custom'),
                Item('ai_color', style='custom'),
                label='Game colors',
                show_border=True
            ),
            HGroup(
                UItem('play_button'),
                UItem('reset_button'),
                show_border=True
            ),
        ),
        resizable=True,
        title='ToonChess',
        icon=ImageResource(get_share_path() + 'logo.png')
    )

if __name__ == '__main__':
    ToonChess().configure_traits()
