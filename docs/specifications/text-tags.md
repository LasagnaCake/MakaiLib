# Text Tags

## Overview

- Tags can be nested within each other.

## Syntax

`[`*`chr-seq`*`:`*`text`*`:]`, where:

- *`chr-seq`*: A sequence of [tags](#Tags) (as detailed below) defining the style of the proceeding *`text`*.
- *`text`*: A string of text.

`[` and `]` can be escaped as `[[` and `]]`, respectively.

## Valid Tags

Spaces are ignored.

`+` and `-` tags are $\frac{1}{2}$ of the size of the font size.

|Tag|Usage|Can be Stacked?|Additional Information|
|:-:|:-|:-:|:-|
|`b`|**bold**|<span style="background-color:lightgreen;padding:0 5px;">Yes</span>|
|`i`|*italic*|<span style="background-color:lightgreen;padding:0 5px;">Yes</span>|
|`u`|<ins>underline</ins>|<span style="background-color:lightpink;padding:0 5px;">No</span>|
|`s`|~~strikethrough~~|<span style="background-color:lightpink;padding:0 5px;">No</span>|
|`+`|<sup>superscript</sup>|<span style="background-color:lightgreen;padding:0 5px;">Yes</span>|
|`-`|<sub>subscript</sub>|<span style="background-color:lightgreen;padding:0 5px;">Yes</span>|
|`@[link]`|[links](https://www.youtube.com/watch?v=ihCc2MoLF9k)|<span style="background-color:lightpink;padding:0 5px;">No</span>|
|`@`|[links](https://www.youtube.com/watch?v=ihCc2MoLF9k)|<span style="background-color:lightpink;padding:0 5px;">No</span>|Link shorthand, where `text` value is the link.|
|`$`|JSON Value|<span style="background-color:lightpink;padding:0 5px;">No</span>|</p>To be loaded from a JSON structure attached to a JSON database attached to the `Label`, with `text` being the key.</p><p>Tree structure is separated by periods.</p>|
|`#[style]`|for (limited) text stylization|<span style="background-color:lightgreen;padding:0 5px;">Yes</span>|Most recent value set for parameter is used (CSS-style).|

### On the `#[style]` tag

`style` parameters are of the form `parameter:value`. For multiple values, each value is separated via a comma.

Optionally, it can contain parameters in the form of `@class`, that takes no value. This denotes a style class to be used.

Every parameter must be separated via semicolons.

To use a JSON value, do it as `$[key]`, with `key` following the same principles as the `=` tag.

#### Available Parameters

|Name|Usage|Value Types|Function|
|:-:|:-|:-|:-:|
|`color`|`color:COLOR`|`COLOR`: string or name|Sets the text color.|
|`font`|`font:FONT`|`FONT`: string|Sets the text to a font attached to a font database.|
|`animate`|`animate:NAMES,...`|`NAMES`: string or name|<p>Sets the text to a specific animation.</p><p>Can be a built-in one, or a user-defined one.</p>|

##### Available Built-in Animations

|Name|Input|Input Types|Function|Infinite|
|:-:|:-|:-|:-:|:-:|
|`shake`|`shake:I`|`I`: float|Displaces the text randomly, from its starting position, at an intensity `I`.|Yes|

## Examples

|Text Tag|Resulting Text|
|:-:|:-|
|`[biu:abcdefg:]`|<ins>***abcdefg***</ins>|
|`[#[color:#f00]:colored text:]`|<span style="color:red">colored text</span>|
|`[#[color:red]:colored text:]`|<span style="color:red">colored text</span>|
|`[@:http://example.com:]`|[http://example.com](http://example.com)|
|`[@[http://example.com]::]`|[http://example.com](http://example.com)|
|`[bi@[http://example.com]::]`|[***http://example.com***](http://example.com)|
|`[bi@[http://example.com]:click me!:]`|[***click me!***](http://example.com)|
|`[:text:]`|text|
|`[::text:]`|:text|
|`[:text::]`|text:|
|`[::text::]`|:text:|
|`[text]`|\[text\]|
|`[`*`any-chr-seq`[^1]*`::]`||
|`[::]`||
|`[::]]`|:\]|
|`[[::]`|\[::]|
|`[[::]]`|\[::\]|
|`[:[[::]`|\[:|
|`[:::]`|:|

[^1]: Except links (`@`).
