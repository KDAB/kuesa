#version 330

const vec2 _451[113] = vec2[](vec2(-0.0), vec2(0.09407208859920501708984375, 0.0), vec2(-0.098098099231719970703125, 0.0898658931255340576171875), vec2(0.014244941063225269317626953125, -0.162313759326934814453125), vec2(0.114474229514598846435546875, 0.149311363697052001953125), vec2(-0.20713603496551513671875, -0.0366394482553005218505859375), vec2(0.19442535936832427978515625, -0.123677499592304229736328125), vec2(-0.06461326777935028076171875, 0.24035812914371490478515625), vec2(-0.122636318206787109375, -0.23612876236438751220703125), vec2(0.2650917470455169677734375, 0.096811078488826751708984375), vec2(-0.2749762237071990966796875, 0.11350618302822113037109375), vec2(0.132240712642669677734375, -0.28259074687957763671875), vec2(0.097529210150241851806640625, 0.31093847751617431640625), vec2(-0.29346382617950439453125, -0.1700682938098907470703125), vec2(0.3437757194042205810546875, -0.075578130781650543212890625), vec2(-0.20954243838787078857421875, 0.2980525493621826171875), vec2(-0.048357076942920684814453125, -0.3731682002544403076171875), vec2(0.2965837419033050537109375, 0.2499611079692840576171875), vec2(-0.398773252964019775390625, 0.01649052463471889495849609375), vec2(0.2906560003757476806640625, -0.2892415523529052734375), vec2(-0.01943288743495941162109375, 0.42025411128997802734375), vec2(-0.276204884052276611328125, -0.3309857547283172607421875), vec2(0.437296688556671142578125, 0.0588376522064208984375), vec2(-0.3703334331512451171875, 0.2576683461666107177734375), vec2(0.10114957392215728759765625, -0.4496200084686279296875), vec2(0.23385421931743621826171875, 0.4081067740917205810546875), vec2(-0.4569832980632781982421875, -0.1457901298999786376953125), vec2(0.44374001026153564453125, -0.2050191462039947509765625), vec2(-0.1921745240688323974609375, 0.45919120311737060546875), vec2(-0.17145778238773345947265625, -0.4766963422298431396484375), vec2(0.456097424030303955078125, 0.2397120296955108642578125), vec2(-0.50647103786468505859375, 0.13350413739681243896484375), vec2(0.287807941436767578125, -0.447607457637786865234375), vec2(0.091531388461589813232421875, 0.532594978809356689453125), vec2(-0.4336803853511810302734375, -0.335866451263427734375), vec2(0.55463778972625732421875, -0.04595060646533966064453125), vec2(-0.38329446315765380859375, 0.4143300950527191162109375), vec2(0.0027914387173950672149658203125, -0.57221138477325439453125), vec2(0.389625728130340576171875, 0.4295054972171783447265625), vec2(-0.584973037242889404296875, -0.054215170443058013916015625), vec2(0.47392427921295166015625, -0.3596916496753692626953125), vec2(-0.10781170427799224853515625, 0.59262847900390625), vec2(-0.324706614017486572265625, -0.51599133014678955078125), vec2(0.5949342250823974609375, 0.1630468070507049560546875), vec2(-0.555167496204376220703125, 0.2849027812480926513671875), vec2(0.21936523914337158203125, -0.591700077056884765625), vec2(0.24056375026702880859375, 0.590938866138458251953125), vec2(-0.582790911197662353515625, -0.2761954963207244873046875), vec2(0.622821152210235595703125, -0.1920224130153656005859375), vec2(-0.3329546451568603515625, 0.568128108978271484375), vec2(-0.1396582424640655517578125, -0.650364100933074951171875), vec2(0.54769039154052734375, 0.3890535831451416015625), vec2(-0.673155844211578369140625, 0.083894111216068267822265625), vec2(0.443902194499969482421875, -0.52151453495025634765625), vec2(0.02519268356263637542724609375, 0.6908266544342041015625), vec2(-0.48969519138336181640625, -0.4969147741794586181640625), vec2(0.70305263996124267578125, 0.035947553813457489013671875), vec2(-0.547515392303466796875, 0.452384471893310546875), vec2(0.0989966094493865966796875, -0.709559023380279541015625), vec2(0.40979087352752685546875, 0.595143139362335205078125), vec2(-0.710123002529144287109375, -0.1633976399898529052734375), vec2(0.639257609844207763671875, -0.362177789211273193359375), vec2(-0.22857187688350677490234375, 0.704576075077056884765625), vec2(-0.3098615705966949462890625, -0.679343760013580322265625), vec2(0.692806243896484375, 0.2939237654209136962890625), vec2(-0.714917123317718505859375, 0.2532089650630950927734375), vec2(0.3588463366031646728515625, -0.67475926876068115234375), vec2(0.19263420999050140380859375, 0.745528280735015869140625), vec2(-0.6504399776458740234375, -0.4227266013622283935546875), vec2(0.77076756954193115234375, -0.1285956799983978271484375), vec2(-0.4849512577056884765625, 0.619912326335906982421875), vec2(-0.0615920759737491607666015625, -0.790268957614898681640625), vec2(0.583299934864044189453125, 0.54491221904754638671875), vec2(-0.8037140369415283203125, -0.007841759361326694488525390625), vec2(0.602012217044830322265625, -0.540785133838653564453125), vec2(-0.07913933694362640380859375, 0.81083524227142333984375), vec2(-0.492608010768890380859375, -0.6556704044342041015625), vec2(0.811419010162353515625, 0.15170736610889434814453125), vec2(-0.70532786846160888671875, 0.43906497955322265625), vec2(0.2249308526515960693359375, -0.8053081035614013671875), vec2(0.380506455898284912109375, 0.750452816486358642578125), vec2(-0.792403757572174072265625, -0.2981784641742706298828125), vec2(0.790545761585235595703125, -0.3173344433307647705078125), vec2(-0.3708080351352691650390625, 0.772667229175567626953125), vec2(-0.249999463558197021484375, -0.825144290924072265625), vec2(0.7461206912994384765625, 0.442172229290008544921875), vec2(-0.853827893733978271484375, 0.17899699509143829345703125), vec2(0.51162421703338623046875, -0.712847888469696044921875), vec2(0.104863584041595458984375, 0.876221835613250732421875), vec2(-0.67299401760101318359375, -0.578523695468902587890625), vec2(0.8920013904571533203125, -0.02817252092063426971435546875), vec2(-0.642242372035980224609375, 0.626765072345733642578125), vec2(0.050470829010009765625, -0.90089511871337890625), vec2(0.574426949024200439453125, 0.702169895172119140625), vec2(-0.902687847614288330078125, -0.1304340362548828125), vec2(0.75771939754486083984375, -0.516303479671478271484375), vec2(-0.21106289327144622802734375, 0.89722347259521484375), vec2(-0.452774465084075927734375, -0.808333098888397216796875), vec2(0.8844068050384521484375, 0.29168689250946044921875), vec2(-0.853487193584442138671875, 0.384273052215576171875), vec2(0.3716249763965606689453125, -0.864205181598663330078125), vec2(0.3112823665142059326171875, 0.892697393894195556640625), vec2(-0.8366496562957763671875, -0.4501912891864776611328125), vec2(0.9255230426788330078125, -0.2343319952487945556640625), vec2(-0.526701271533966064453125, 0.801835238933563232421875), vec2(-0.15399388968944549560546875, -0.95157206058502197265625), vec2(0.759921014308929443359375, 0.600477397441864013671875), vec2(-0.9705045223236083984375, 0.070877902209758758544921875), vec2(0.67085552215576171875, -0.7111294269561767578125), vec2(-0.014373018406331539154052734375, 0.98203623294830322265625), vec2(-0.655745208263397216796875, -0.737190306186676025390625), vec2(0.98594224452972412109375, 0.101088054478168487548828125), vec2(-0.798861443996429443359375, 0.59411346912384033203125));

uniform float nearPlane;
uniform float farPlane;
uniform sampler2D depthTexture;
uniform float focusDistance;
uniform float focusRange;
uniform float bokehRadius;
uniform vec2 textureSize;
uniform sampler2D textureSampler;

in vec2 texCoord;
layout(location = 0) out vec4 fragColor;

float mapZ(float z)
{
    float a = ((-1.0) + (nearPlane / farPlane)) / nearPlane;
    float b = 1.0 / nearPlane;
    return 1.0 / ((a * z) + b);
}

float confusionCircle()
{
    float param = texture(depthTexture, texCoord).x;
    float depth = mapZ(param);
    float coc = (depth - focusDistance) / focusRange;
    coc = clamp(coc, -1.0, 1.0) * bokehRadius;
    return coc;
}

float weigh(float coc, float radius)
{
    return clamp(((coc - radius) + 2.0) / 2.0, 0.0, 1.0);
}

vec4 depthOfField()
{
    vec3 color = vec3(0.0);
    float weight = 0.0;
    float confCircle = abs(confusionCircle());
    for (int k = 0; k < 113; k++)
    {
        vec2 sampledPoint = _451[k] * bokehRadius;
        float radius = length(sampledPoint);
        sampledPoint /= textureSize;
        vec4 s = texture(textureSampler, texCoord + sampledPoint);
        float param = confCircle;
        float param_1 = radius;
        float sw = weigh(param, param_1);
        color += (s.xyz * sw);
        weight += sw;
    }
    return vec4(color / vec3(weight), 1.0);
}

void main()
{
    fragColor = depthOfField();
}

