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

`+` and `-` tags are $\frac{1}{4}$ of the size of the font size.

|Tag|Usage|Can be Stacked?|Additional Info|
|:-:|:-|:-:|:-|
|`b`|**bold**|Yes|
|`i`|*italic*|Yes|
|`u`|<ins>underline</ins>|No|
|`s`|~~strikethrough~~|No|
|`+`|<sup>superscript</sup>|Yes|
|`-`|<sub>subscript</sub>|Yes|
|`@[link]`|[links](https://www.youtube.com/watch?v=ihCc2MoLF9k)|No|
|`@`|[links](https://www.youtube.com/watch?v=ihCc2MoLF9k)|No|Link shorthand, where `text` value is the link.|
|`=`|JSON Value|No|</p>To be loaded from a JSON structure attached to the `Label`, with `text` being the key.</p><p>Tree structure is separated by periods.</p>|
|`#[style]`|for (limited) text stylization|Yes|Most recent value set for parameter is used (CSS-style).|

### On the `#[style]` tag

`style` parameters are of the form `parameter=value`, and are separated via semicolons.

Optionally, it can start with `@:`, and be a list of style names attached to the label. Each name must be separated by a semicolon.

#### Available parameters

- `color`: Takes in a hex color (ex: `#00f`, `#f045cf0f`, `#fc0d`) as value
- `font`: Takes in an unsigned integer as a value to be used as an index into an array of fonts


## Examples

|Text Tag|Resulting Text|
|:-:|:-|
|`[biu:abcdefg:]`|<ins>***abcdefg***</ins>|
|`[+#[color=#f00]:colored text:]`|<span style="color:red"><sup>colored text</sup></span>|
|`[@:http://example.com:]`|[http://example.com](http://example.com)|
|`[@[http://example.com]::]`|[http://example.com](http://example.com)|
|`[bi@[http://example.com]::]`|[***http://example.com***](http://example.com)|
|`[bi@[http://example.com]:click me!:]`|[***click me!***](http://example.com)|
|`[:text:]`|text|
|`[::text:]`|:text|
|`[:text::]`|text:|
|`[::text::]`|:text:|
|`[text]`|\[text\]|
|`[`*`any-chr-seq`*`::]`||
|`[::]`||
|`[::]]`|:\]|
|`[[::]`|\[::]|
|`[[::]]`|\[::\]|
|`[:[[::]`|\[:|
|`[:::]`|:|