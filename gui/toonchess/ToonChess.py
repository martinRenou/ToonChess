import subprocess
from concurrent import futures

from traits.api import (
    Button, Bool, Color, Instance, HasStrictTraits, Str, Enum, on_trait_change
)
from traitsui.api import Item, UItem, VGroup, HGroup, View
from pyface.image_resource import ImageResource

from get_share_path import get_share_path
from utils import get_config, set_config

default = {
    'resolution': 'fullscreen',
    'shadows': 'high',
    'antialiasing': 'high',
    'difficulty': 'easy',
    'ai': 'stockfish',
    'user_pieces_color': '255,237,178',
    'user_smoke_color': '105,94,59',
    'ai_pieces_color': '130,20,20',
    'ai_smoke_color': '77,31,102',
    'background_color': '255,255,255',
    'board_color_1': '179,153,105',
    'board_color_2': '255,255,255',
    'allowed_move_color': '240,207,87',
}


class ToonChess(HasStrictTraits):
    resolution = Enum('fullscreen', '1920/1600', '1024/576')
    shadows = Enum('high', 'low', 'very low')
    antialiasing = Enum('high', 'low', 'none')

    difficulty = Enum('impossible', 'hard', 'normal', 'easy')
    ai = Str('stockfish')

    user_pieces_color = Color()
    ai_pieces_color = Color()
    user_smoke_color = Color()
    ai_smoke_color = Color()
    background_color = Color()
    board_color_1 = Color()
    board_color_2 = Color()
    allowed_move_color = Color()

    play_button = Button("Play")
    reset_button = Button("Reset default")

    _game_running = Bool(False)
    _executor = Instance(futures.ThreadPoolExecutor)

    traits_view = View(
        VGroup(
            HGroup(
                Item('resolution'),
                Item('shadows'),
                Item('antialiasing'),
                label='Graphical settings',
                show_border=True
            ),
            '20',
            HGroup(
                Item('ai', label='AI'),
                Item('difficulty'),
                label='AI settings',
                show_border=True
            ),
            '20',
            VGroup(
                HGroup(
                    VGroup(
                        Item('user_pieces_color', label='Pieces'),
                        Item('user_smoke_color', label='Smoke'),
                        label='User',
                        show_border=True
                    ),
                    VGroup(
                        Item('ai_pieces_color', label='Pieces'),
                        Item('ai_smoke_color', label='Smoke'),
                        label='AI',
                        show_border=True
                    ),
                ),
                Item('background_color', label='Background'),
                Item('board_color_1'),
                Item('board_color_2'),
                Item('allowed_move_color'),
                label='Game colors',
                show_border=True
            ),
            HGroup(
                UItem('play_button', enabled_when=("not _game_running")),
                UItem('reset_button'),
                show_border=True
            ),
            show_border=True
        ),
        resizable=False,
        title='ToonChess',
        icon=ImageResource(get_share_path() + 'logo.png')
    )

    def __init__(self, **traits):
        config = get_config()

        values = {
            key: config.get(key, value)
            for key, value in default.items()
        }
        self.set(**values)

        super(ToonChess, self).__init__(**traits)

    @on_trait_change('play_button')
    def _on_play_click(self):
        set_config(self.trait_get())

        self._game_running = True
        future = self._executor.submit(self._play)
        future.add_done_callback(self._game_done)

    def _play(self):
        process = subprocess.Popen("ToonChess")
        out, err = process.communicate()

    def _game_done(self, future):
        self._game_running = False

    def __executor_default(self):
        return futures.ThreadPoolExecutor(max_workers=1)

    @on_trait_change('reset_button')
    def _on_reset_click(self):
        self.set(**default)

if __name__ == '__main__':
    ToonChess().configure_traits()
